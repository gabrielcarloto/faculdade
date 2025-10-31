package main

const (
	MAX_ITER = 10
	MAX_ERR  = 1e-4
)

func main() {
	model_h1 := readMatrix("./models/H-1.csv")
	// model_h2 := readMatrix("./models/H-2.csv")

	signal := readVector("./signals/G-1.csv")

	image, iterations, duration := CGNE(model_h1, signal)

	printCPUUsage()
	printMemoryUsage()

	saveImageWithInfo(image, "./out/teste.png", ImageInfo{
		Algorithm:  "CGNE",
		Iterations: iterations,
		Duration:   duration,
	})
}
