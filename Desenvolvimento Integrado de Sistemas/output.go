package main

import (
	"encoding/json"
	"fmt"
	"image"
	"image/color"
	"image/png"
	"math"
	"os"
	"time"

	"gonum.org/v1/gonum/mat"
)

type ReconstructionInfo struct {
	Iterations int     `json:"iterations"`
	StartTime  string  `json:"start_time"`
	EndTime    string  `json:"end_time"`
	Duration   float64 `json:"duration_ms"`
	Algorithm  string  `json:"algorithm"`
	TaskID     uint32  `json:"task_id"`
	Dimensions string  `json:"dimensions"`
}

func SaveReconstructionResult(
	taskID uint32,
	algorithm string,
	dimensions string,
	imageVec *mat.VecDense,
	iterations int,
	start time.Time,
	end time.Time,
	outputDir string,
) error {
	if err := os.MkdirAll(outputDir, 0755); err != nil {
		return fmt.Errorf("erro ao criar diretório de saída: %w", err)
	}

	imagePath := fmt.Sprintf("%s/%d_image.png", outputDir, taskID)
	if err := saveImageAsPNG(imageVec, imagePath); err != nil {
		return fmt.Errorf("erro ao salvar imagem: %w", err)
	}

	duration := end.Sub(start).Milliseconds()
	info := ReconstructionInfo{
		Iterations: iterations,
		StartTime:  start.Format(time.RFC3339Nano),
		EndTime:    end.Format(time.RFC3339Nano),
		Duration:   float64(duration),
		Algorithm:  algorithm,
		TaskID:     taskID,
		Dimensions: dimensions,
	}

	jsonPath := fmt.Sprintf("%s/%d_info.json", outputDir, taskID)
	if err := saveInfoAsJSON(info, jsonPath); err != nil {
		return fmt.Errorf("erro ao salvar informações: %w", err)
	}

	return nil
}

func saveImageAsPNG(imageVec *mat.VecDense, filePath string) error {
	length := imageVec.Len()
	size := int(math.Sqrt(float64(length)))

	if size*size != length {
		return fmt.Errorf("o vetor de imagem não representa uma imagem quadrada: length=%d", length)
	}

	data := imageVec.RawVector().Data
	minVal, maxVal := data[0], data[0]
	for _, v := range data {
		if v < minVal {
			minVal = v
		}
		if v > maxVal {
			maxVal = v
		}
	}

	img := image.NewGray(image.Rect(0, 0, size, size))

	for i := 0; i < length; i++ {
		row := i / size
		col := i % size

		val := data[i]
		normalized := uint8(0)
		if maxVal > minVal {
			normalized = uint8(((val - minVal) / (maxVal - minVal)) * 255)
		}

		img.SetGray(col, row, color.Gray{Y: normalized})
	}

	file, err := os.Create(filePath)
	if err != nil {
		return err
	}
	defer file.Close()

	if err := png.Encode(file, img); err != nil {
		return err
	}

	return nil
}

func saveInfoAsJSON(info ReconstructionInfo, filePath string) error {
	file, err := os.Create(filePath)
	if err != nil {
		return err
	}
	defer file.Close()

	encoder := json.NewEncoder(file)
	encoder.SetIndent("", "  ")

	if err := encoder.Encode(info); err != nil {
		return err
	}

	return nil
}
