package main

import (
	"encoding/json"
	"fmt"
	"os"
	"os/signal"
	"runtime"
	"strings"
	"syscall"
	"time"
)

func printMemoryUsage() {
	var stats runtime.MemStats
	runtime.ReadMemStats(&stats)

	fmt.Printf("Memória em uso     : %s\n", formatBytes(stats.Alloc))
	fmt.Printf("Total alocado      : %s\n", formatBytes(stats.TotalAlloc))
	fmt.Printf("Reservado do sistema: %s\n", formatBytes(stats.Sys))
	fmt.Printf("Rodadas de GC      : %d\n", stats.NumGC)
	fmt.Printf("Tempo total de GC  : %v\n\n", time.Duration(stats.PauseTotalNs))
}

func printCPUUsage() {
	numCPU := runtime.NumCPU()
	numGoroutines := runtime.NumGoroutine()

	fmt.Printf("CPUs disponíveis   : %d\n", numCPU)
	fmt.Printf("Goroutines ativas  : %d\n", numGoroutines)
}

func formatBytes(bytes uint64) string {
	const (
		KB = 1024
		MB = 1024 * KB
		GB = 1024 * MB
	)

	switch {
	case bytes >= GB:
		return fmt.Sprintf("%.2f GB", float64(bytes)/float64(GB))
	case bytes >= MB:
		return fmt.Sprintf("%.2f MB", float64(bytes)/float64(MB))
	case bytes >= KB:
		return fmt.Sprintf("%.2f KB", float64(bytes)/float64(KB))
	default:
		return fmt.Sprintf("%d B", bytes)
	}
}

type Snapshot struct {
	Time            int64  `json:"time"`
	CurrentMemUsage uint64 `json:"currentMemUsage"`
	TotalAlloc      uint64 `json:"totalAlloc"`
	Reserved        uint64 `json:"reserved"`
	GcRounds        uint32 `json:"gcRounds"`
	TotalGCTime     uint64 `json:"totalGCTime"`
	CurrentRoutines int    `json:"currentRoutines"`
}

func resourceSnapshot() Snapshot {
	var stats runtime.MemStats
	runtime.ReadMemStats(&stats)
	numGoroutines := runtime.NumGoroutine()

	return Snapshot{
		Time:            time.Now().Unix(),
		CurrentMemUsage: stats.Alloc,
		TotalAlloc:      stats.TotalAlloc,
		Reserved:        stats.Sys,
		GcRounds:        stats.NumGC,
		TotalGCTime:     stats.PauseTotalNs,
		CurrentRoutines: numGoroutines,
	}
}

var (
	snapshots        []Snapshot
	monitoringActive bool
)

func watchResources(interval time.Duration) {
	monitoringActive = true
	go func() {
		for monitoringActive {
			snapshot := resourceSnapshot()
			snapshots = append(snapshots, snapshot)
			time.Sleep(interval)
		}
	}()
}

func stopMonitoring() {
	monitoringActive = false
}

func exportReportJSON(filename string) error {
	file, err := os.Create(filename)
	if err != nil {
		return err
	}
	defer file.Close()

	encoder := json.NewEncoder(file)
	encoder.SetIndent("", "  ")
	return encoder.Encode(snapshots)
}

func printPerformanceReport() {
	if len(snapshots) == 0 {
		fmt.Println("⚠️  Nenhum snapshot coletado")
		return
	}

	fmt.Println("\n" + strings.Repeat("═", 80))
	fmt.Println("📊 RELATÓRIO DE PERFORMANCE")
	fmt.Println(strings.Repeat("═", 80))

	first := snapshots[0]
	last := snapshots[len(snapshots)-1]
	duration := time.Duration(last.Time-first.Time) * time.Second

	fmt.Printf("\n⏱️  Duração do monitoramento: %v\n", duration)
	fmt.Printf("📸 Total de snapshots: %d\n", len(snapshots))
	fmt.Printf("🔄 Intervalo entre snapshots: ~%.1fs\n\n", duration.Seconds()/float64(len(snapshots)))

	fmt.Println("💾 MEMÓRIA")
	fmt.Println(strings.Repeat("─", 80))
	printMemoryChart("Memória em Uso", snapshots, func(s Snapshot) float64 {
		return float64(s.CurrentMemUsage) / (1024 * 1024)
	})

	fmt.Println("\n🗑️  GARBAGE COLLECTOR")
	fmt.Println(strings.Repeat("─", 80))
	fmt.Printf("Total de rodadas de GC: %d → %d (Δ %d)\n",
		first.GcRounds, last.GcRounds, last.GcRounds-first.GcRounds)
	fmt.Printf("Tempo total de GC: %v → %v (Δ %v)\n\n",
		time.Duration(first.TotalGCTime),
		time.Duration(last.TotalGCTime),
		time.Duration(last.TotalGCTime-first.TotalGCTime))

	fmt.Println("🔀 GOROUTINES")
	fmt.Println(strings.Repeat("─", 80))
	printSimpleChart("Goroutines Ativas", snapshots, func(s Snapshot) float64 {
		return float64(s.CurrentRoutines)
	})

	fmt.Println("\n📈 RESUMO")
	fmt.Println(strings.Repeat("─", 80))
	avgMem := calculateAverage(snapshots, func(s Snapshot) float64 {
		return float64(s.CurrentMemUsage)
	})
	maxMem := calculateMax(snapshots, func(s Snapshot) float64 {
		return float64(s.CurrentMemUsage)
	})
	minMem := calculateMin(snapshots, func(s Snapshot) float64 {
		return float64(s.CurrentMemUsage)
	})

	fmt.Printf("Memória Mínima    : %s\n", formatBytes(uint64(minMem)))
	fmt.Printf("Memória Média     : %s\n", formatBytes(uint64(avgMem)))
	fmt.Printf("Memória Máxima    : %s\n", formatBytes(uint64(maxMem)))
	fmt.Printf("Memória Final     : %s\n", formatBytes(last.CurrentMemUsage))
	fmt.Println(strings.Repeat("═", 80))
}

func printMemoryChart(title string, data []Snapshot, getValue func(Snapshot) float64) {
	if len(data) == 0 {
		return
	}

	const width = 60
	const height = 10

	min := getValue(data[0])
	max := getValue(data[0])
	for _, snap := range data {
		val := getValue(snap)
		if val < min {
			min = val
		}
		if val > max {
			max = val
		}
	}

	if max == min {
		max = min + 1
	}

	fmt.Printf("%s (MB)\n", title)

	for row := height - 1; row >= 0; row-- {
		threshold := min + (max-min)*float64(row+1)/float64(height)
		label := fmt.Sprintf("%6.0f │ ", threshold)
		fmt.Print(label)

		for col := 0; col < width; col++ {
			idx := col * len(data) / width
			if idx >= len(data) {
				idx = len(data) - 1
			}
			val := getValue(data[idx])

			if val >= threshold {
				fmt.Print("█")
			} else {
				fmt.Print(" ")
			}
		}
		fmt.Println()
	}

	// Eixo X
	fmt.Print("       └" + strings.Repeat("─", width) + "\n")
	fmt.Printf("        0%s%d snapshots%s100%%\n", strings.Repeat(" ", 20), len(data), strings.Repeat(" ", 20))
}

func printSimpleChart(title string, data []Snapshot, getValue func(Snapshot) float64) {
	if len(data) == 0 {
		return
	}

	min := getValue(data[0])
	max := getValue(data[0])
	avg := 0.0

	for _, snap := range data {
		val := getValue(snap)
		avg += val
		if val < min {
			min = val
		}
		if val > max {
			max = val
		}
	}
	avg /= float64(len(data))

	fmt.Printf("%s: %.0f (min) → %.0f (avg) → %.0f (max)\n", title, min, avg, max)
}

func calculateAverage(data []Snapshot, getValue func(Snapshot) float64) float64 {
	sum := 0.0
	for _, snap := range data {
		sum += getValue(snap)
	}
	return sum / float64(len(data))
}

func calculateMax(data []Snapshot, getValue func(Snapshot) float64) float64 {
	max := getValue(data[0])
	for _, snap := range data {
		val := getValue(snap)
		if val > max {
			max = val
		}
	}
	return max
}

func calculateMin(data []Snapshot, getValue func(Snapshot) float64) float64 {
	min := getValue(data[0])
	for _, snap := range data {
		val := getValue(snap)
		if val < min {
			min = val
		}
	}
	return min
}

func setupSignalHandler() {
	sigChan := make(chan os.Signal, 1)
	signal.Notify(sigChan, os.Interrupt, syscall.SIGTERM)

	go func() {
		<-sigChan
		stopMonitoring()
		time.Sleep(100 * time.Millisecond)

		printPerformanceReport()

		if err := exportReportJSON("performance_report.json"); err != nil {
			fmt.Printf("Erro ao exportar JSON: %v\n", err)
		} else {
			fmt.Println("Relatório JSON exportado: performance_report.json")
		}

		os.Exit(0)
	}()
}
