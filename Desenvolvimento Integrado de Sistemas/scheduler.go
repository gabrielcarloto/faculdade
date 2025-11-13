package main

import (
	"errors"
	"log"
	"math"
	"runtime"
	"slices"
	"sync"
	"time"

	"gonum.org/v1/gonum/mat"
)

var (
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

	queue      []*Task
	queueCond  = sync.NewCond(&sync.Mutex{})
	nextTaskID = uint32(0)

	maxWorkers            = runtime.NumCPU()
	workerSemaphore       = make(chan struct{}, maxWorkers)
	workersWaitGroup      sync.WaitGroup
	retryTimeoutWaitGroup sync.WaitGroup
)

type Task struct {
	ID          uint32
	Algorithm   string
	Signal      []float64
	Priority    float32
	ArrivalTime time.Time
	Retries     uint32
}

func calcPriority(task *Task) float32 {
	resourceFactor := modelLoadedPriority[isModelLoaded(len(task.Signal))]
	algoFactor := algorithmPriority[task.Algorithm]

	waitTime := time.Since(task.ArrivalTime).Seconds()
	starvationBonus := float32(math.Log1p(waitTime)) * 2

	retryPenalty := float32(math.Pow(0.9, float64(task.Retries)))

	priority := (resourceFactor * algoFactor * retryPenalty) + starvationBonus

	return min(priority, 10_000)
}

func EnqueueTask(request ReconstructionRequest) (*Task, error) {
	_, modelExists := models[len(request.Signal)]

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
		Priority:    algorithmPriority[request.Algorithm],
		ArrivalTime: time.Now(),
	}

	nextTaskID++

	queue = append(queue, task)

	return task, nil
}

func sortQueue() {
	slices.SortStableFunc(queue, func(a, b *Task) int {
		a.Priority, b.Priority = calcPriority(a), calcPriority(b)
		return int(b.Priority - a.Priority)
	})
}

func scheduler() {
	for {
		queueCond.L.Lock()
		for len(queue) == 0 {
			queueCond.Wait()
		}

		sortQueue()
		task := queue[0]
		queue = queue[1:]
		queueCond.L.Unlock()

		reserved := tryReserveModel(len(task.Signal))

		if !reserved {
			task.Retries++
			task.Priority *= 0.9

			retryTimeoutWaitGroup.Go(func() {
				backoff := time.Duration(task.Retries) * 100 * time.Millisecond
				time.Sleep(backoff)
				queueCond.L.Lock()
				queue = append(queue, task)
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

	model, _ := LoadModel(signalLen)
	defer model.release()

	reconstructImage := algorithmMap[task.Algorithm]
	signal := mat.NewVecDense(signalLen, task.Signal)
	task.Signal = nil

	image, iterations, start, end := reconstructImage(model.matrix, signal)

	SaveReconstructionResult(task.ID, task.Algorithm, model.dimensions, image, iterations, start, end, "./out")
}

func InitScheduler() {
	go scheduler()
}
