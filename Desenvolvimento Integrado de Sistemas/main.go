package main

import (
	"encoding/json"
	"fmt"
	"net/http"
	"time"

	"gonum.org/v1/gonum/mat"
)

type ReconstructionRequest struct {
	Dimensions string    `json:"dimensions"`
	Algorithm  string    `json:"algorithm"`
	Signal     []float64 `json:"signal"`
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

func main() {
	watchResources(150 * time.Millisecond)
	setupSignalHandler()

	modelByDimension := map[string]*mat.Dense{
		"60x60": loadModel("H-1"),
		"30x30": loadModel("H-2"),
	}

	algorithmMap := map[string]ReconstructionAlgo{
		"CGNE": CGNE,
		"CGNR": CGNR,
	}

	http.HandleFunc("/reconstruct", func(res http.ResponseWriter, req *http.Request) {
		if req.Method != http.MethodPost {
			http.Error(res, "MethodNotAllowed", http.StatusMethodNotAllowed)
			return
		}

		var reconstructionRequest ReconstructionRequest

		if err := json.NewDecoder(req.Body).Decode(&reconstructionRequest); err != nil {
			http.Error(res, err.Error(), http.StatusBadRequest)
			return
		}

		model := modelByDimension[reconstructionRequest.Dimensions]
		signal := mat.NewVecDense(len(reconstructionRequest.Signal), reconstructionRequest.Signal)

		image, iterations, start, end := algorithmMap[reconstructionRequest.Algorithm](model, signal)

		response := ReconstructionResponse{
			Image:      image.RawVector().Data,
			Dimensions: reconstructionRequest.Dimensions,
			Algorithm:  reconstructionRequest.Algorithm,
			Iterations: iterations,
			Start:      start,
			End:        end,
			Duration:   end.Sub(start),
		}

		res.Header().Set("Content-Type", "application/json")
		res.WriteHeader(http.StatusOK)

		json.NewEncoder(res).Encode(response)
	})

	fmt.Println("Escutando na porta 3000")
	http.ListenAndServe(":3000", nil)
}
