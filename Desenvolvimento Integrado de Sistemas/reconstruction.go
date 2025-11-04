package main

import (
	"time"

	"gonum.org/v1/gonum/mat"
)

const (
	MaxIter = 10
	MaxErr  = 1e-4
)

func CGNE(model *mat.Dense, signal *mat.VecDense) (*mat.VecDense, int, time.Duration) {
	startTime := time.Now()

	rows, cols := model.Dims()

	// f_0 = 0
	image := mat.NewVecDense(cols, nil)

	// g = signal
	// r_0 = g - H * f_0 = g (f_0 = 0)
	residual := mat.VecDenseCopyOf(signal)

	// p_0 = H^T * r_0
	p := mat.NewVecDense(cols, nil)
	p.MulVec(model.T(), residual)

	// guardar o valor anterior evita ter que fazer cópias de r
	var previousResidualNorm float64
	var i int

	// NOTE: criar vetores alphaHp, alphaP e HTr antes do loop aparentemente não
	// muda muito a performance. Na verdade, por algum motivo usa mais memória.
	for i = 0; i < MaxIter; i++ {
		// alpha_i = (r_i^T * r_i) / (p_i^T * p_i)
		rTr := mat.Dot(residual, residual)
		pTp := mat.Dot(p, p)
		alpha := rTr / pTp

		// f_(i+1) = f_i + alpha_i * p_i
		alphaP := mat.NewVecDense(cols, nil)
		alphaP.ScaleVec(alpha, p)
		image.AddVec(image, alphaP)

		// r_(i+1) = r_i - alpha_i * H * p_i
		alphaHp := mat.NewVecDense(rows, nil)
		alphaHp.MulVec(model, p)
		alphaHp.ScaleVec(alpha, alphaHp)
		residual.SubVec(residual, alphaHp)

		// cálculo do erro
		// como o valor de r não altera mais, podemos fazer a verificação aqui
		// epsilon = || r_(i+1) ||_2  -  || r_i ||_2
		currentResidualNorm := mat.Norm(residual, 2)

		if i > 0 {
			epsilon := currentResidualNorm - previousResidualNorm

			if epsilon < MaxErr {
				break
			}
		}

		previousResidualNorm = currentResidualNorm

		// Beta_i = (r_(i+1)^T * r_(i+1)) / (r_i^T * r_i)
		// (r_i^T * r_i) = rTr
		rTrNext := mat.Dot(residual, residual)
		beta := rTrNext / rTr

		// p_(i+1) = H^T * r_(i+1) + Beta_i * p_i
		HTr := mat.NewVecDense(cols, nil)
		HTr.MulVec(model.T(), residual)
		p.AddScaledVec(HTr, beta, p)
	}

	duration := time.Since(startTime)
	iterations := i + 1

	return image, iterations, duration
}

// func CGNR(model *mat.Dense, signal *mat.VecDense) (*mat.VecDense, int, time.Duration) {
// }
