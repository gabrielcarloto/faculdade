package main

import (
	"encoding/binary"
	"encoding/csv"
	"io"
	"log"
	"math"
	"os"
	"strconv"
	"unsafe"

	"github.com/james-bowman/sparse"
	"github.com/klauspost/compress/zstd"
	"gonum.org/v1/gonum/mat"
)

type Matrix struct {
	H  mat.Matrix
	HT mat.Matrix
}

func readMatrixCSV(filename string) (*Matrix, error) {
	rows, cols, data, err := readCSV(filename)
	if err != nil {
		return nil, err
	}

	dense := mat.NewDense(rows, cols, data)
	return &Matrix{H: dense, HT: dense.T()}, nil
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

	totalFloats := rows * cols
	totalBytes := totalFloats * 8

	bytes := make([]byte, totalBytes)

	// NOTE: leitura completa como bytes é mais rápida do que deixar o Go
	// converter para floats
	if _, err := io.ReadFull(zstdReader, bytes); err != nil {
		return -1, -1, nil, err
	}

	ptr := unsafe.Pointer(&bytes[0])
	// Cast de bytes para floats 🤯🤯
	floats := unsafe.Slice((*float64)(ptr), totalFloats)

	return int(rows), int(cols), floats, nil
}

func readSparseBinary(filename string) (*Matrix, error) {
	file, err := os.Open(filename)
	if err != nil {
		return nil, err
	}
	defer file.Close()

	decoder, err := zstd.NewReader(file)
	if err != nil {
		return nil, err
	}
	defer decoder.Close()

	header := make([]int64, 3)
	if err := binary.Read(decoder, binary.LittleEndian, header); err != nil {
		return nil, err
	}
	rows, cols, nnz := int(header[0]), int(header[1]), int(header[2])

	sizeOfInt := int(unsafe.Sizeof(int(0)))

	rowPtrBytes := make([]byte, (rows+1)*sizeOfInt)
	if _, err := io.ReadFull(decoder, rowPtrBytes); err != nil {
		return nil, err
	}
	rowPtr := unsafe.Slice((*int)(unsafe.Pointer(&rowPtrBytes[0])), rows+1)

	colIdxBytes := make([]byte, nnz*sizeOfInt)
	if _, err := io.ReadFull(decoder, colIdxBytes); err != nil {
		return nil, err
	}
	colIdx := unsafe.Slice((*int)(unsafe.Pointer(&colIdxBytes[0])), nnz)

	valuesBytes := make([]byte, nnz*8)
	if _, err := io.ReadFull(decoder, valuesBytes); err != nil {
		return nil, err
	}
	values := unsafe.Slice((*float64)(unsafe.Pointer(&valuesBytes[0])), nnz)

	csr := sparse.NewCSR(rows, cols, rowPtr, colIdx, values)
	csc := sparse.NewCSC(cols, rows, rowPtr, colIdx, values)

	return &Matrix{H: csr, HT: csc}, nil
}

func saveSparseBinary(filename string, matrix *sparse.CSR) error {
	f, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer f.Close()

	enc, _ := zstd.NewWriter(f)
	defer enc.Close()
	writer := enc

	rows, cols := matrix.Dims()
	nnz := matrix.NNZ()

	header := []int64{int64(rows), int64(cols), int64(nnz)}
	if err := binary.Write(writer, binary.LittleEndian, header); err != nil {
		return err
	}

	raw := matrix.RawMatrix()

	indptr64 := unsafe.Slice((*int64)(unsafe.Pointer(&raw.Indptr[0])), len(raw.Indptr))
	if err := binary.Write(writer, binary.LittleEndian, indptr64); err != nil {
		return err
	}

	indices64 := unsafe.Slice((*int64)(unsafe.Pointer(&raw.Ind[0])), len(raw.Ind))
	if err := binary.Write(writer, binary.LittleEndian, indices64); err != nil {
		return err
	}

	if err := binary.Write(writer, binary.LittleEndian, raw.Data); err != nil {
		return err
	}

	return nil
}

func convertToSparse(model string) {
	matrix, err := readMatrixBinary("./models/" + model + ".mat")
	if err != nil {
		log.Fatalf("Não foi possível converter o modelo %s: %s", model, err)
	}

	dense := matrix.H.(*mat.Dense)

	rows, cols := dense.Dims()

	raw := dense.RawMatrix()
	data := raw.Data
	stride := raw.Stride

	values := make([]float64, 0)
	colIdx := make([]int, 0)
	rowPtr := make([]int, rows+1)

	const epsilon = 1e-12

	currentNNZ := 0
	rowPtr[0] = 0

	for r := range rows {
		// Onde começa a linha 'r' no slice bruto
		offset := r * stride

		for c := range cols {
			val := data[offset+c]

			if math.Abs(val) > epsilon {
				values = append(values, val)
				colIdx = append(colIdx, c)
				currentNNZ++
			}
		}
		rowPtr[r+1] = currentNNZ
	}

	csr := sparse.NewCSR(rows, cols, rowPtr, colIdx, values)

	saveSparseBinary("./models/"+model+".csr", csr)
}

func readMatrixBinary(filename string) (*Matrix, error) {
	rows, cols, data, err := readBinary(filename)
	if err != nil {
		return nil, err
	}

	dense := mat.NewDense(int(rows), int(cols), data)

	return &Matrix{H: dense, HT: dense.T()}, nil
}

func loadModel(nameWithoutExt string) *Matrix {
	path := "./models/" + nameWithoutExt
	binaryPath := path + ".mat"
	sparsePath := path + ".csr"

	sparseFileInfo, err := os.Stat(sparsePath)
	isSparse := err == nil

	if isSparse && sparseFileInfo.Size() > 0 {
		matrix, err := readSparseBinary(sparsePath)

		if err == nil {
			return matrix
		}

		log.Printf("Erro ao ler %s sparse: %s", nameWithoutExt, err)
	}

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
	if err := saveMatrixBinary(binaryPath, matrix.H.(*mat.Dense)); err != nil {
		log.Printf("Erro ao salvar binário: %s", err)
	} else {
		log.Printf("Matriz salva")
	}

	return matrix
}
