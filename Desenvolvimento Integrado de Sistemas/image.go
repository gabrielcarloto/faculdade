package main

import (
	"fmt"
	"image"
	"image/color"
	"image/draw"
	"image/png"
	"math"
	"os"
	"time"

	"golang.org/x/image/font"
	"golang.org/x/image/font/basicfont"
	"golang.org/x/image/math/fixed"
	"gonum.org/v1/gonum/mat"
)

type ImageInfo struct {
	Algorithm  string
	Iterations int
	Duration   time.Duration
}

func saveImageWithInfo(f *mat.VecDense, filename string, info ImageInfo) {
	n := f.Len()

	var matrixWidth = 60

	scale := 4
	scaledWidth := matrixWidth * scale
	scaledHeight := matrixWidth * scale

	headerHeight := 80
	padding := 20
	finalWidth := scaledWidth + (2 * padding)
	finalHeight := scaledHeight + headerHeight + (2 * padding)

	finalImg := image.NewRGBA(image.Rect(0, 0, finalWidth, finalHeight))
	draw.Draw(finalImg, finalImg.Bounds(), &image.Uniform{color.White}, image.Point{}, draw.Src)

	minVal, maxVal := f.AtVec(0), f.AtVec(0)
	for i := range n {
		val := f.AtVec(i)
		if val < minVal {
			minVal = val
		}
		if val > maxVal {
			maxVal = val
		}
	}

	for i := range n {
		x := i % matrixWidth
		y := i / matrixWidth

		val := f.AtVec(i)
		normalized := (val - minVal) / (maxVal - minVal)
		if math.IsNaN(normalized) || math.IsInf(normalized, 0) {
			normalized = 0
		}
		grayValue := uint8(normalized * 255)

		for sy := range scale {
			for sx := range scale {
				px := padding + (x * scale) + sx
				py := headerHeight + padding + (y * scale) + sy
				finalImg.Set(px, py, color.Gray{Y: grayValue})
			}
		}
	}

	col := color.Black
	point := fixed.Point26_6{X: fixed.I(padding), Y: fixed.I(25)}

	addLabel(finalImg, point, fmt.Sprintf("Algoritmo: %s", info.Algorithm), col)
	point.Y += fixed.I(20)
	addLabel(finalImg, point, fmt.Sprintf("Iteracoes: %d", info.Iterations), col)
	point.Y += fixed.I(20)
	addLabel(finalImg, point, fmt.Sprintf("Tempo: %v", info.Duration.Round(time.Millisecond)), col)

	file, err := os.Create(filename)
	if err != nil {
		panic(err)
	}
	defer file.Close()

	if err := png.Encode(file, finalImg); err != nil {
		panic(err)
	}
}

func addLabel(img *image.RGBA, point fixed.Point26_6, label string, col color.Color) {
	d := &font.Drawer{
		Dst:  img,
		Src:  image.NewUniform(col),
		Face: basicfont.Face7x13,
		Dot:  point,
	}
	d.DrawString(label)
}
