package main

import (
	"log"
	"sync"
	"time"

	"gonum.org/v1/gonum/mat"
)

type Model struct {
	name            string
	matrix          *mat.Dense
	currentlyUsing  int
	estimatedMemory uint64
	dimensions      string
}

var modelsByRows = map[int]*Model{
	50816: {name: "H-1", matrix: nil, currentlyUsing: 0, estimatedMemory: 50816 * (60 * 60) * 8, dimensions: "60x60"},
	27904: {name: "H-2", matrix: nil, currentlyUsing: 0, estimatedMemory: 27904 * (30 * 30) * 8, dimensions: "30x30"},
}

var mutex sync.Mutex

func LoadModel(rows int) (*mat.Dense, string, func()) {
	model, ok := modelsByRows[rows]

	if !ok {
		log.Fatalf("Modelo não encontrado: %d", rows)
	}

	unloadModelTimeout := func() {
		mutex.Lock()
		if model.currentlyUsing == 0 {
			log.Printf("Descarregando modelo %d\n", rows)
			model.matrix = nil
		}
		mutex.Unlock()
	}

	releaseFn := func() {
		mutex.Lock()
		model.currentlyUsing--
		mutex.Unlock()

		go func() {
			time.Sleep(5 * time.Second)
			unloadModelTimeout()
		}()
	}

	mutex.Lock()
	log.Printf("Atualmente usando modelo %d: %d\n", rows, model.currentlyUsing)
	model.currentlyUsing++

	if model.matrix == nil {
		log.Printf("Carregando modelo %d\n", rows)
		model.matrix = loadModel(model.name)
	}

	mutex.Unlock()
	return model.matrix, model.dimensions, releaseFn
}
