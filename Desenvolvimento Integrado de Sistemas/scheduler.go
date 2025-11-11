package main

import (
	"errors"
	"log"
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

	maxWorkers       = runtime.NumCPU()
	workerSemaphore  = make(chan struct{}, maxWorkers)
	workersWaitGroup sync.WaitGroup
)

type Task struct {
	ID          uint32
	Algorithm   string
	Signal      []float64
	Priority    float32
	ArrivalTime time.Time
}

func calcPriority(task *Task) float32 {
	resourceFactor := modelLoadedPriority[isModelLoaded(len(task.Signal))]
	starvationFactor := time.Since(task.ArrivalTime).Seconds() / 3

	return task.Priority * resourceFactor * float32(starvationFactor)
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
		reserved := tryReserveModel(len(task.Signal))

		for !reserved {
			task.Priority *= 0.8
			queue = append(queue, task)

			task = queue[0]
			queue = queue[1:]
			reserved = tryReserveModel(len(task.Signal))
		}

		queueCond.L.Unlock()

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
	image, iterations, start, end := reconstructImage(model.matrix, signal)

	SaveReconstructionResult(task.ID, task.Algorithm, model.dimensions, image, iterations, start, end, "./out")
}

func InitScheduler() {
	go scheduler()
}
