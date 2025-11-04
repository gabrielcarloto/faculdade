package main

import "log"

func main() {
	modelH1 := loadModel("H-1")

	signal, err := readVectorCSV("./signals/G-2.csv")
	if err != nil {
		log.Fatalf("Não foi possível ler o signal: %s", err)
	}

	image, iterations, duration := CGNE(modelH1, signal)

	printCPUUsage()
	printMemoryUsage()

	saveImageWithInfo(image, "./out/teste.png", ImageInfo{
		Algorithm:  "CGNE",
		Iterations: iterations,
		Duration:   duration,
	})
}
