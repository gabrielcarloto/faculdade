package main

import (
	"encoding/json"
	"fmt"
	"net/http"
	"runtime/debug"
	"time"

	"gonum.org/v1/gonum/blas/blas64"
)

type ReconstructionRequest struct {
	Algorithm string    `json:"algorithm"`
	Signal    []float64 `json:"signal"`
}

type PyClientRequest struct {
	Algorithm string    `json:"algoritmo"`
	Signal    []float64 `json:"g"`
}

type ReconstructionResponse struct {
	Image      []float64     `json:"image"`
	Dimensions string        `json:"dimensions"`
	Algorithm  string        `json:"algorithm"`
	Iterations int           `json:"iterations"`
	Start      time.Time     `json:"start"`
	End        time.Time     `json:"end"`
	Duration   time.Duration `json:"duration"`
}

type AsyncReconstructionResponse struct {
	TaskID uint32 `json:"taskId"`
}

var cache ModelCache

func main() {
	watchResources(150 * time.Millisecond)
	setupSignalHandler()

	// blas64.Use(netlib.Implementation{})

	implStruct := blas64.Implementation()
	fmt.Printf("Implementação BLAS utilizada: %T\n", implStruct)

	if DEBUG {
		fmt.Println("Build atual: DEBUG")
	} else {
		fmt.Println("Build atual: PROD")
	}

	mem, _ := GetMemoryUsage()

	debug.SetMemoryLimit(int64(mem.Available * 90 / 100))
	maxMem := mem.Available * 75 / 100

	cache.Init(maxMem)
	InitScheduler(maxMem)

	http.HandleFunc("/reconstruir/", func(res http.ResponseWriter, req *http.Request) {
		if req.Method != http.MethodPost {
			http.Error(res, "MethodNotAllowed", http.StatusMethodNotAllowed)
			return
		}

		var reconstructionRequest PyClientRequest

		if err := json.NewDecoder(req.Body).Decode(&reconstructionRequest); err != nil {
			http.Error(res, err.Error(), http.StatusBadRequest)
			return
		}

		createdTask, err := EnqueueTask(ReconstructionRequest(reconstructionRequest))
		if err != nil {
			http.Error(res, err.Error(), http.StatusBadRequest)
			return
		}

		res.Header().Set("Content-Type", "application/json")
		res.WriteHeader(http.StatusAccepted)

		response := AsyncReconstructionResponse{
			TaskID: createdTask.ID,
		}

		json.NewEncoder(res).Encode(response)
	})

	fmt.Println("Escutando na porta 8000")
	http.ListenAndServe(":8000", nil)
}
