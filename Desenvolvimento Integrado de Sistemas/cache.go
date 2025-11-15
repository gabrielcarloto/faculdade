package main

import (
	"fmt"
	"log"
	"runtime"
	"sync"
	"time"

	"gonum.org/v1/gonum/mat"
)

type Model struct {
	Name                    string
	Matrix                  *mat.Dense
	Dimensions              string
	reservations            int
	reservartionAttempts    int
	firstReservationAttempt *time.Time
	estimatedMemory         uint64
	isLoading               bool
	loadCond                *sync.Cond
}

type Rows = int

type CachedModels = map[Rows]*Model

type ModelCache struct {
	currentMemoryUsage uint64
	memoryLimit        uint64
	models             CachedModels
	mutex              sync.Mutex
	starvingModel      *Model
}

func (cache *ModelCache) Init(maxMem uint64) {
	cache.models = CachedModels{
		50816: {
			Name:            "H-1",
			Matrix:          nil,
			reservations:    0,
			estimatedMemory: 50816 * (60 * 60) * 8 * 1.01,
			Dimensions:      "60x60",
			isLoading:       false,
			loadCond:        sync.NewCond(&cache.mutex),
		},
		27904: {
			Name:            "H-2",
			Matrix:          nil,
			reservations:    0,
			estimatedMemory: 27904 * (30 * 30) * 8 * 1.01,
			Dimensions:      "30x30",
			isLoading:       false,
			loadCond:        sync.NewCond(&cache.mutex),
		},
	}

	cache.starvingModel = nil

	minMemoryRequirements := uint64(0)

	for _, model := range cache.models {
		if model.estimatedMemory > minMemoryRequirements {
			minMemoryRequirements = model.estimatedMemory
		}
	}

	cache.memoryLimit = maxMem * 75 / 100
	cache.currentMemoryUsage = 0

	if cache.memoryLimit < minMemoryRequirements {
		log.Fatalf("memória insuficiente")
	}
}

func (cache *ModelCache) reserve(rows Rows) (bool, error) {
	model, ok := cache.models[rows]

	if !ok {
		return false, fmt.Errorf("can't load an unregistered model")
	}

	isStarving := cache.starvingModel != nil &&
		time.Since(*cache.starvingModel.firstReservationAttempt) > 10*time.Second

	if isStarving && model != cache.starvingModel {
		// log.Printf("Negando reserva do modelo %s por inanição do %s", model.Name, cache.starvingModel.Name)
		return false, nil
	}

	if model.Matrix != nil || model.reservations > 0 {
		return true, nil
	}

	newUsage := model.estimatedMemory + cache.currentMemoryUsage

	if newUsage <= cache.memoryLimit {
		cache.currentMemoryUsage = newUsage
		return true, nil
	}

	if couldFree := cache.evictMemory(model.estimatedMemory); !couldFree {
		return false, nil
	}

	cache.currentMemoryUsage += model.estimatedMemory
	return true, nil
}

func (cache *ModelCache) TryReserve(rows Rows) (bool, error) {
	cache.mutex.Lock()
	defer cache.mutex.Unlock()

	reserved, err := cache.reserve(rows)
	if err != nil {
		return false, err
	}

	model := cache.models[rows]

	if reserved {
		model.reservations++
		model.firstReservationAttempt = nil

		if cache.starvingModel == model {
			cache.starvingModel = nil
		}
	} else if model.firstReservationAttempt == nil && model.Matrix == nil {
		now := time.Now()
		model.firstReservationAttempt = &now

		isMostStarved := cache.starvingModel == nil ||
			model.firstReservationAttempt.Before(*cache.starvingModel.firstReservationAttempt)

		if isMostStarved {
			cache.starvingModel = model
		}
	}

	return reserved, err
}

func (cache *ModelCache) Load(rows Rows) (*Model, error) {
	model, ok := cache.models[rows]

	if !ok {
		return nil, fmt.Errorf("model not found: %d", rows)
	}

	if model.reservations == 0 {
		return nil, fmt.Errorf("attempt to load unreserved model: %d", rows)
	}

	cache.mutex.Lock()
	defer cache.mutex.Unlock()

	if model.Matrix == nil && !model.isLoading {
		model.isLoading = true
		cache.mutex.Unlock()

		log.Printf("Carregando modelo: %d\n", rows)
		matrix := loadModel(model.Name)
		log.Printf("Modelo carregado: %d\n", rows)

		cache.mutex.Lock()
		model.Matrix = matrix
		model.isLoading = false
		model.loadCond.Broadcast()
		go UpdatePriorities()
	}

	for model.isLoading {
		model.loadCond.Wait()
	}

	return model, nil
}

func (cache *ModelCache) Release(rows Rows) {
	cache.mutex.Lock()
	defer cache.mutex.Unlock()

	model := cache.models[rows]
	model.reservations--

	isCandidate := model.reservations == 0 && model.Matrix != nil

	if isCandidate {
		go func() {
			time.Sleep(5 * time.Second)
			cache.mutex.Lock()
			defer cache.mutex.Unlock()

			shouldFree := model.reservations == 0 && model.Matrix != nil

			if shouldFree {
				model.Matrix = nil
				cache.currentMemoryUsage -= model.estimatedMemory
				log.Printf("Modelo liberado: %d", rows)
				go UpdatePriorities()
			}
		}()
	}
}

func (cache *ModelCache) IsModelReserved(rows Rows) bool {
	model, ok := cache.models[rows]
	return ok && model.reservations > 0
}

func (cache *ModelCache) ModelExists(rows Rows) bool {
	_, ok := cache.models[rows]
	return ok
}

func (cache *ModelCache) evictMemory(target uint64) bool {
	memoryThatCanBeFreed := uint64(0)
	var modelsToFree []int

	for key, model := range cache.models {
		if model.Matrix != nil && model.reservations == 0 {
			memoryThatCanBeFreed += model.estimatedMemory
			modelsToFree = append(modelsToFree, key)
		}
	}

	if memoryThatCanBeFreed < target {
		return false
	}

	for _, key := range modelsToFree {
		cache.models[key].Matrix = nil
	}

	// TODO: creio que não seria necessário rodar GC manualmente caso
	// o que ficasse no cache fosse um slice fora do heap do Go,
	// mas como meu pc aguenta os dois modelos, por enquanto vou manter assim
	runtime.GC()
	cache.currentMemoryUsage -= memoryThatCanBeFreed

	return true
}
