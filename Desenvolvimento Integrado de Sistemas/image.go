package main

import (
	"image"
	"image/color"
	"image/png"
	"os"

	"gonum.org/v1/gonum/mat"
)

func saveImage(f *mat.VecDense, filename string) {
	width := 60
	height := 60

	img := image.NewGray(image.Rect(0, 0, width, height))

	minVal, maxVal := f.AtVec(0), f.AtVec(0)
	for i := 0; i < f.Len(); i++ {
		val := f.AtVec(i)
		if val < minVal {
			minVal = val
		}
		if val > maxVal {
			maxVal = val
		}
	}

	for i := 0; i < f.Len(); i++ {
		x := i % width
		y := i / width

		val := f.AtVec(i)
		normalized := (val - minVal) / (maxVal - minVal)
		grayValue := uint8(normalized * 255)

		img.SetGray(x, y, color.Gray{Y: grayValue})
	}

	file, err := os.Create(filename)
	if err != nil {
		panic(err)
	}
	defer file.Close()

	if err := png.Encode(file, img); err != nil {
		panic(err)
	}
}
