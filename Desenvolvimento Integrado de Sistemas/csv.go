package main

import (
	"encoding/csv"
	"io"
	"log"
	"os"
	"strconv"

	"gonum.org/v1/gonum/mat"
)

func readMatrix(filename string) *mat.Dense {
	file, err := os.Open(filename)
	if err != nil {
		log.Fatalf("Não foi possível abrir o arquivo %s. Erro: %s", filename, err)
	}
	defer file.Close()

	reader := csv.NewReader(file)

	var data []float64
	rows := 0
	cols := 0

	for {
		record, err := reader.Read()
		if err == io.EOF {
			break
		}
		if err != nil {
			log.Fatalf("Não foi possível ler o arquivo %s. Erro: %s", filename, err)
		}

		if cols == 0 {
			cols = len(record)
		}

		for _, val := range record {
			float, err := strconv.ParseFloat(val, 64)
			if err != nil {
				log.Fatalf("Não foi possível converter '%s' para float64: %s", val, err)
			}
			data = append(data, float)
		}
		rows++

		if rows%10000 == 0 {
			log.Printf("Lidas %d linhas de %s...", rows, filename)
		}
	}

	if rows == 0 {
		log.Fatalf("O arquivo %s está vazio", filename)
	}

	log.Printf("Arquivo %s completamente lido: %d linhas x %d colunas", filename, rows, cols)
	return mat.NewDense(rows, cols, data)
}

func readVector(filename string) *mat.VecDense {
	file, err := os.Open(filename)
	if err != nil {
		log.Fatalf("Não foi possível abrir o arquivo %s. Erro: %s", filename, err)
	}
	defer file.Close()

	reader := csv.NewReader(file)

	var data []float64
	rows := 0

	for {
		record, err := reader.Read()
		if err == io.EOF {
			break
		}
		if err != nil {
			log.Fatalf("Não foi possível ler o arquivo %s. Erro: %s", filename, err)
		}

		for _, val := range record {
			float, err := strconv.ParseFloat(val, 64)
			if err != nil {
				log.Fatalf("Não foi possível converter '%s' para float64: %s", val, err)
			}
			data = append(data, float)
		}
		rows++

		if rows%10000 == 0 {
			log.Printf("Lidas %d linhas de %s...", rows, filename)
		}
	}

	if len(data) == 0 {
		log.Fatalf("O arquivo %s está vazio", filename)
	}

	log.Printf("Arquivo %s completamente lido: %d elementos totais", filename, len(data))
	return mat.NewVecDense(len(data), data)
}
