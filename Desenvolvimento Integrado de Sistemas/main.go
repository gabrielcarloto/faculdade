package main

import (
	"encoding/json"
	"fmt"
	"net/http"
	"time"

	"gonum.org/v1/gonum/mat"
)

type ReconstructionRequest struct {
	Algorithm string    `json:"algorithm"`
	Signal    []float64 `json:"signal"`
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

	modelH1 := loadModel("H-1")
	modelH2 := loadModel("H-2")

	modelsBySignalLen := map[int]*mat.Dense{
		modelH1.RawMatrix().Rows: modelH1,
		modelH2.RawMatrix().Rows: modelH2,
	}

	dimensionsBySignalLen := map[int]string{
		modelH1.RawMatrix().Rows: "60x60",
		modelH2.RawMatrix().Rows: "30x30",
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

		signalLen := len(reconstructionRequest.Signal)

		model, ok := modelsBySignalLen[signalLen]

		if !ok {
			http.Error(res, "InvalidSignalSize", http.StatusBadRequest)
			return
		}

		algorithm, ok := algorithmMap[reconstructionRequest.Algorithm]

		if !ok {
			http.Error(res, "InvalidAlgorithm", http.StatusBadRequest)
			return
		}

		signal := mat.NewVecDense(signalLen, reconstructionRequest.Signal)

		image, iterations, start, end := algorithm(model, signal)

		response := ReconstructionResponse{
			Image:      image.RawVector().Data,
			Dimensions: dimensionsBySignalLen[signalLen],
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
