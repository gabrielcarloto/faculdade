package main

import (
	"encoding/binary"
	"encoding/csv"
	"io"
	"log"
	"os"
	"strconv"

	"github.com/klauspost/compress/zstd"
	"gonum.org/v1/gonum/mat"
)

func readMatrixCSV(filename string) (*mat.Dense, error) {
	rows, cols, data, err := readCSV(filename)
	if err != nil {
		return nil, err
	}

	return mat.NewDense(rows, cols, data), nil
}

func readCSV(filename string) (int, int, []float64, error) {
	file, err := os.Open(filename)
	if err != nil {
		return -1, -1, nil, err
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
			return -1, -1, nil, err
		}

		if cols == 0 {
			cols = len(record)
		}

		for _, val := range record {
			float, err := strconv.ParseFloat(val, 64)
			if err != nil {
				return -1, -1, nil, err
			}
			data = append(data, float)
		}

		rows++
	}

	if rows == 0 {
		return -1, -1, nil, err
	}

	return rows, cols, data, nil
}

func saveBinary(filename string, rows int, cols int, data []float64) error {
	file, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	zstdWriter, err := zstd.NewWriter(file)
	if err != nil {
		return err
	}
	defer zstdWriter.Close()

	if err := binary.Write(zstdWriter, binary.LittleEndian, int64(rows)); err != nil {
		return err
	}
	if err := binary.Write(zstdWriter, binary.LittleEndian, int64(cols)); err != nil {
		return err
	}

	return binary.Write(zstdWriter, binary.LittleEndian, data)
}

func saveMatrixBinary(filename string, m *mat.Dense) error {
	rows, cols := m.Dims()
	data := m.RawMatrix().Data
	return saveBinary(filename, rows, cols, data)
}

func readBinary(filename string) (int, int, []float64, error) {
	file, err := os.Open(filename)
	if err != nil {
		return -1, -1, nil, err
	}
	defer file.Close()

	zstdReader, err := zstd.NewReader(file)
	if err != nil {
		return -1, -1, nil, err
	}
	defer zstdReader.Close()

	var rows, cols int64
	if err := binary.Read(zstdReader, binary.LittleEndian, &rows); err != nil {
		return -1, -1, nil, err
	}
	if err := binary.Read(zstdReader, binary.LittleEndian, &cols); err != nil {
		return -1, -1, nil, err
	}

	data := make([]float64, rows*cols)
	if err := binary.Read(zstdReader, binary.LittleEndian, data); err != nil {
		return -1, -1, nil, err
	}

	return int(rows), int(cols), data, nil
}

func readMatrixBinary(filename string) (*mat.Dense, error) {
	rows, cols, data, err := readBinary(filename)
	if err != nil {
		return nil, err
	}

	return mat.NewDense(int(rows), int(cols), data), nil
}

func loadModel(nameWithoutExt string) *mat.Dense {
	path := "./models/" + nameWithoutExt
	binaryPath := path + ".mat"

	binFileInfo, err := os.Stat(binaryPath)
	isBinary := err == nil

	if isBinary && binFileInfo.Size() > 0 {
		matrix, err := readMatrixBinary(binaryPath)

		if err == nil {
			return matrix
		}

		log.Printf("Erro ao ler %s binário: %s", nameWithoutExt, err)
	}

	matrix, err := readMatrixCSV(path + ".csv")
	if err != nil {
		log.Fatalf("Não foi possível ler o modelo %s: %s", nameWithoutExt, err)
	}

	log.Printf("Salvando %s como binário...", nameWithoutExt)
	if err := saveMatrixBinary(binaryPath, matrix); err != nil {
		log.Printf("Erro ao salvar binário: %s", err)
	} else {
		log.Printf("Matriz salva")
	}

	return matrix
}
