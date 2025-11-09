package main

import (
	"fmt"
	"log"
	"runtime"
	"sync"
	"time"

	"github.com/shirou/gopsutil/v4/mem"
	"gonum.org/v1/gonum/mat"
)

type CachedModel struct {
	name            string
	matrix          *mat.Dense
	currentlyUsing  int
	estimatedMemory uint64
	dimensions      string
}

var models = map[int]*CachedModel{
	50816: {name: "H-1", matrix: nil, currentlyUsing: 0, estimatedMemory: 50816 * (60 * 60) * 8, dimensions: "60x60"},
	27904: {name: "H-2", matrix: nil, currentlyUsing: 0, estimatedMemory: 27904 * (30 * 30) * 8, dimensions: "30x30"},
}

var mutex sync.Mutex

type Model struct {
	matrix     *mat.Dense
	dimensions string
}

func (model *Model) release() {
	mutex.Lock()
	defer mutex.Unlock()

	cachedModel := models[model.matrix.RawMatrix().Rows]
	cachedModel.currentlyUsing--

	currentlyUsing := cachedModel.currentlyUsing

	go func() {
		time.Sleep(5 * time.Second)
		mutex.Lock()
		defer mutex.Unlock()
		if cachedModel.currentlyUsing == 0 && currentlyUsing == 0 {
			cachedModel.matrix = nil
			// TODO: como garantir que a memória foi liberada?
		}
	}()
}

func tryLoadModel(rows int) (*Model, error) {
	model, ok := models[rows]

	if !ok {
		return nil, fmt.Errorf("Modelo não encontrado: %d", rows)
	}

	mutex.Lock()
	defer mutex.Unlock()

	if canLoad, err := canLoadModel(rows); !canLoad || err != nil {
		return nil, fmt.Errorf("Modelo não pode ser carregado agora: %d", rows)
	}

	model.currentlyUsing++

	if model.matrix == nil {
		log.Printf("Carregando modelo %d\n", rows)
		model.matrix = loadModel(model.name)
	}

	publicModel := new(Model)

	publicModel.matrix = model.matrix
	publicModel.dimensions = model.dimensions

	return publicModel, nil
}

func canLoadModel(rows int) (bool, error) {
	model, ok := models[rows]

	if !ok {
		return false, fmt.Errorf("modelo não encontrado: %d", rows)
	}

	if model.matrix != nil {
		return true, nil
	}

	mem := mem.NewExWindows()
	vmem, err := mem.VirtualMemory()
	if err != nil {
		return false, err
	}

	var memStats runtime.MemStats
	runtime.ReadMemStats(&memStats)

	heapAvailable := memStats.HeapIdle - memStats.HeapReleased

	canLoad := model.estimatedMemory < (vmem.PhysAvail*85/100 + heapAvailable)

	if canLoad {
		return true, nil
	}

	memoryThatCanBeFreed := uint64(0)
	var modelsToFree []int

	for key, model := range models {
		if model.matrix != nil && model.currentlyUsing == 0 {
			memoryThatCanBeFreed += model.estimatedMemory
			modelsToFree = append(modelsToFree, key)
		}
	}

	if memoryThatCanBeFreed < model.estimatedMemory {
		return false, nil
	}

	for key := range modelsToFree {
		models[key].matrix = nil
	}

	// TODO: creio que não seria necessário rodar GC manualmente caso
	// o que ficasse no cache fosse um slice fora do heap do Go,
	// mas como meu pc aguenta os dois modelos, por enquanto vou manter assim
	runtime.GC()

	return true, nil
}
