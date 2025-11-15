package main

import (
	"container/heap"
	"errors"
	"log"
	"math"
	"runtime"
	"sync"
	"time"
	"unsafe"

	"gonum.org/v1/gonum/mat"
)

type Task struct {
	ID          uint32
	Algorithm   string
	Signal      []float64
	Priority    float32
	ArrivalTime time.Time
	Retries     uint32
	index       int
}

type PriorityQueue []*Task

func (q PriorityQueue) Len() int { return len(q) }

func (q PriorityQueue) Less(i, j int) bool {
	if q[i].Priority == q[j].Priority {
		return q[i].ArrivalTime.Before(q[j].ArrivalTime)
	}

	return q[i].Priority > q[j].Priority
}

func (q PriorityQueue) Swap(i, j int) {
	q[i], q[j] = q[j], q[i]
	q[i].index = j
	q[j].index = i
}

func (q *PriorityQueue) Push(x any) {
	task := x.(*Task)
	task.index = len(*q)
	*q = append(*q, task)
}

func (q *PriorityQueue) Pop() any {
	item := (*q)[len(*q)-1]
	*q = (*q)[0 : len(*q)-1]
	return item
}

func (q *PriorityQueue) Update(task *Task, priority float32) {
	task.Priority = priority
	heap.Fix(q, task.index)
}

var (
	maxTasks uint

	algorithmMap = map[string]ReconstructionAlgo{
		"CGNE": CGNE,
		"CGNR": CGNR,
	}

	algorithmPriority = map[string]float32{
		// NOTE: CGNE parece ser mais pesado
		"CGNE": 1.0,
		"CGNR": 1.2,
	}

	modelLoadedPriority = map[bool]float32{
		true:  1.5,
		false: 1.0,
	}

	queue      PriorityQueue
	queueCond  = sync.NewCond(&sync.Mutex{})
	nextTaskID = uint32(0)

	maxWorkers            = runtime.NumCPU()
	workerSemaphore       = make(chan struct{}, maxWorkers)
	workersWaitGroup      sync.WaitGroup
	retryTimeoutWaitGroup sync.WaitGroup
)

func calcBasePriority(task *Task) float32 {
	resourceFactor := modelLoadedPriority[cache.IsModelReserved(len(task.Signal))]
	algoFactor := algorithmPriority[task.Algorithm]

	return resourceFactor * algoFactor
}

func calcPriority(task *Task) float32 {
	base := calcBasePriority(task)

	waitTime := time.Since(task.ArrivalTime).Seconds()
	starvationBonus := float32(math.Log1p(waitTime)) * 2

	retryPenalty := float32(math.Pow(0.9, float64(task.Retries)))

	priority := (base * retryPenalty) + starvationBonus

	return min(priority, 10_000)
}

func UpdatePriorities() {
	queueCond.L.Lock()
	defer queueCond.L.Unlock()

	for _, task := range queue {
		task.Priority = calcPriority(task)
	}

	// TODO: não sei se preciso rodar heapify tantas vezes...
	heap.Init(&queue)
}

func EnqueueTask(request ReconstructionRequest) (*Task, error) {
	if uint(len(queue)) >= maxTasks {
		return nil, errors.New("máximo de tasks atingido")
	}

	modelExists := cache.ModelExists(len(request.Signal))

	if !modelExists {
		return nil, errors.New("não existe modelo correspondente para a requisição")
	}

	_, algorithmExists := algorithmPriority[request.Algorithm]

	if !algorithmExists {
		return nil, errors.New("não existe algoritmo correspondente para a requisição")
	}

	queueCond.L.Lock()
	defer queueCond.L.Unlock()
	defer queueCond.Signal()

	log.Printf("Adicionando task na fila: %d", nextTaskID)

	task := &Task{
		ID:          nextTaskID,
		Algorithm:   request.Algorithm,
		Signal:      request.Signal,
		ArrivalTime: time.Now(),
	}

	task.Priority = calcBasePriority(task)

	nextTaskID++

	heap.Push(&queue, task)

	return task, nil
}

func scheduler() {
	for {
		queueCond.L.Lock()
		for len(queue) == 0 {
			queueCond.Wait()
		}

		task := heap.Pop(&queue).(*Task)
		queueCond.L.Unlock()

		reserved, _ := cache.TryReserve(len(task.Signal))

		if !reserved {
			task.Retries++
			task.Priority *= 0.9

			retryTimeoutWaitGroup.Go(func() {
				backoff := time.Duration(task.Retries) * 100 * time.Millisecond
				time.Sleep(backoff)
				task.Priority = calcPriority(task)
				queueCond.L.Lock()
				heap.Push(&queue, task)
				queueCond.Signal()
				queueCond.L.Unlock()
			})

			continue
		}

		workerSemaphore <- struct{}{}

		workersWaitGroup.Go(func() {
			log.Printf("Iniciando tarefa: %d, prio: %.2f", task.ID, task.Priority)
			runTask(task)
			log.Printf("Tarefa %d encerrou", task.ID)
			<-workerSemaphore
		})
	}
}

func runTask(task *Task) {
	signalLen := len(task.Signal)

	model, _ := cache.Load(signalLen)

	reconstructImage := algorithmMap[task.Algorithm]
	signal := mat.NewVecDense(signalLen, task.Signal)
	task.Signal = nil

	image, iterations, start, end := reconstructImage(model.Matrix, signal)
	cache.Release(signalLen)

	SaveReconstructionResult(task.ID, task.Algorithm, model.Dimensions, image, iterations, start, end, "./out")
}

func InitScheduler(maxMem uint64) {
	taskSize := uint(unsafe.Sizeof(Task{}))
	maxMemory := maxMem * 1 / 50000
	maxTasks = uint(maxMemory) / taskSize

	go scheduler()
	go func() {
		ticker := time.Tick(1 * time.Second)

		for range ticker {
			UpdatePriorities()
		}
	}()
}
