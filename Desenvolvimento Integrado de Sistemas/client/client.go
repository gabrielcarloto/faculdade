package main

import (
	"bufio"
	"bytes"
	"encoding/json"
	"fmt"
	"io"
	"math/rand"
	"net/http"
	"os"
	"strconv"
	"strings"
	"sync"
	"sync/atomic"
	"time"
)

type ClientRequest struct {
	Dimensions string    `json:"dimensions"`
	Algorithm  string    `json:"algorithm"`
	Signal     []float64 `json:"signal"`
}

type ClientResponse struct {
	Image      []float64     `json:"image"`
	Dimensions string        `json:"dimensions"`
	Algorithm  string        `json:"algorithm"`
	Iterations int           `json:"iterations"`
	Start      time.Time     `json:"start"`
	End        time.Time     `json:"end"`
	Duration   time.Duration `json:"duration"`
}

type SignalConfig struct {
	Dimensions string
	Filename   string
	Name       string
}

var (
	algorithms    = []string{"CGNE", "CGNR"}
	signalConfigs = []SignalConfig{
		{Dimensions: "30x30", Filename: "./signals/g-30x30-1.csv", Name: "g-30x30-1"},
		{Dimensions: "30x30", Filename: "./signals/g-30x30-2.csv", Name: "g-30x30-2"},
		{Dimensions: "30x30", Filename: "./signals/A-30x30-1.csv", Name: "A-30x30-1"},
		{Dimensions: "60x60", Filename: "./signals/A-60x60-1.csv", Name: "A-60x60-1"},
		{Dimensions: "60x60", Filename: "./signals/G-1.csv", Name: "G-1"},
		{Dimensions: "60x60", Filename: "./signals/G-2.csv", Name: "G-2"},
	}
)

func main() {
	serverURL := "http://localhost:3000/reconstruct"

	fmt.Printf("Configurações de sinais disponíveis:\n")
	for _, config := range signalConfigs {
		fmt.Printf("  - %s: %s\n", config.Dimensions, config.Name)
	}
	fmt.Println()

	// Configuração inicial: 1 requisição por segundo
	requestsPerSecond := 10
	duration := 10 * time.Second

	fmt.Printf("Iniciando teste leve: %d requisição/segundo por %v\n", requestsPerSecond, duration)
	fmt.Println("Pressione Ctrl+C para parar")
	fmt.Println()

	ticker := time.NewTicker(time.Second / time.Duration(requestsPerSecond))
	defer ticker.Stop()

	timeout := time.After(duration)
	totalRequests := 0
	var successfulRequests int64
	var failedRequests int64
	var wg sync.WaitGroup

	startTime := time.Now()

	for {
		select {
		case <-timeout:
			ticker.Stop()

			fmt.Println("\n⏳ Aguardando requisições em andamento finalizarem...")
			wg.Wait()

			elapsed := time.Since(startTime)
			successful := atomic.LoadInt64(&successfulRequests)
			failed := atomic.LoadInt64(&failedRequests)

			fmt.Println("\n=== Resultados ===")
			fmt.Printf("Duração total: %v\n", elapsed)
			fmt.Printf("Total de requisições enviadas: %d\n", totalRequests)
			fmt.Printf("Requisições bem-sucedidas: %d\n", successful)
			fmt.Printf("Requisições falhas: %d\n", failed)
			fmt.Printf("Taxa de sucesso: %.2f%%\n", float64(successful)/float64(totalRequests)*100)
			fmt.Printf("Requisições/segundo (envio): %.2f\n", float64(totalRequests)/duration.Seconds())
			fmt.Printf("Throughput real: %.2f req/s\n", float64(successful)/elapsed.Seconds())
			return

		case <-ticker.C:
			totalRequests++
			wg.Add(1)

			// Escolhe uma configuração aleatória
			config := signalConfigs[rand.Intn(len(signalConfigs))]

			// Escolhe um algoritmo aleatório
			algorithm := algorithms[rand.Intn(len(algorithms))]

			// Envia a requisição
			go func(reqNum int, cfg SignalConfig, algo string) {
				defer wg.Done()

				// Carrega o sinal do CSV
				signal, err := loadSignalFromCSV(cfg.Filename)
				if err != nil {
					fmt.Printf("❌ Req #%d falhou ao carregar sinal: %v\n", reqNum, err)
					atomic.AddInt64(&failedRequests, 1)
					return
				}

				err = sendRequest(serverURL, cfg, signal, algo, reqNum)
				if err != nil {
					fmt.Printf("❌ Req #%d falhou: %v\n", reqNum, err)
					atomic.AddInt64(&failedRequests, 1)
				} else {
					atomic.AddInt64(&successfulRequests, 1)
				}
			}(totalRequests, config, algorithm)
		}
	}
}

func sendRequest(serverURL string, config SignalConfig, signal []float64, algorithm string, reqNum int) error {
	request := ClientRequest{
		Dimensions: config.Dimensions,
		Algorithm:  algorithm,
		Signal:     signal,
	}

	jsonData, err := json.Marshal(request)
	if err != nil {
		return fmt.Errorf("erro ao serializar JSON: %w", err)
	}

	startTime := time.Now()
	resp, err := http.Post(serverURL, "application/json", bytes.NewBuffer(jsonData))
	if err != nil {
		return fmt.Errorf("erro ao enviar requisição: %w", err)
	}
	defer resp.Body.Close()

	if resp.StatusCode != http.StatusOK {
		body, _ := io.ReadAll(resp.Body)
		return fmt.Errorf("status %d: %s", resp.StatusCode, string(body))
	}

	var response ClientResponse
	if err := json.NewDecoder(resp.Body).Decode(&response); err != nil {
		return fmt.Errorf("erro ao decodificar resposta: %w", err)
	}

	duration := time.Since(startTime)
	fmt.Printf("✓ Req #%d: %s (%s) com %s - %d iterações - %v\n",
		reqNum,
		config.Name,
		config.Dimensions,
		algorithm,
		response.Iterations,
		duration,
	)

	filename := fmt.Sprintf("./out/%s-%s-%d-%d.png", config.Name, algorithm, reqNum, time.Now().UnixNano())

	saveImageWithInfo(response.Image, filename, ImageInfo{
		Algorithm:  response.Algorithm,
		Iterations: response.Iterations,
		Duration:   duration,
	})

	return nil
}

func loadSignalFromCSV(filename string) ([]float64, error) {
	file, err := os.Open(filename)
	if err != nil {
		return nil, fmt.Errorf("erro ao abrir arquivo %s: %w", filename, err)
	}
	defer file.Close()

	var signal []float64
	scanner := bufio.NewScanner(file)

	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" {
			continue
		}

		value, err := strconv.ParseFloat(line, 64)
		if err != nil {
			return nil, fmt.Errorf("erro ao converter valor '%s': %w", line, err)
		}

		signal = append(signal, value)
	}

	if err := scanner.Err(); err != nil {
		return nil, fmt.Errorf("erro ao ler arquivo: %w", err)
	}

	return signal, nil
}
