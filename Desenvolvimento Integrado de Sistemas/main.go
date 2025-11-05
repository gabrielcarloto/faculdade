package main

import (
	"log"
	"time"
)

func main() {
	setupSignalHandler()

	watchResources(150 * time.Millisecond)

	modelH1 := loadModel("H-1")

	signal, err := readVectorCSV("./signals/G-2.csv")
	if err != nil {
		log.Fatalf("Não foi possível ler o signal: %s", err)
	}

	image, iterations, start, end := CGNR(modelH1, signal)

	saveImageWithInfo(image, "./out/teste_cgnr.png", ImageInfo{
		Algorithm:  "CGNR",
		Iterations: iterations,
		Duration:   end.Sub(start),
	})

	stopMonitoring()
	time.Sleep(100 * time.Millisecond)

	printPerformanceReport()

	log.Println("\nExportando relatórios de performance...")
	if err := exportReportJSON("performance_report.json"); err != nil {
		log.Printf("Erro ao exportar JSON: %v\n", err)
	} else {
		log.Println("Relatório JSON exportado: performance_report.json")
	}
}
