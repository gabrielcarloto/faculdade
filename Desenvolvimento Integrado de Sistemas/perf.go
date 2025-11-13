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

	"github.com/shirou/gopsutil/v4/cpu"
	"github.com/shirou/gopsutil/v4/mem"
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
	Time             int64   `json:"time"`
	GoHeapAlloc      uint64  `json:"goHeapAlloc"`
	GoTotalAlloc     uint64  `json:"goTotalAlloc"`
	GoSysReserved    uint64  `json:"goSysReserved"`
	GcRounds         uint32  `json:"gcRounds"`
	TotalGCTime      uint64  `json:"totalGCTime"`
	NumGoroutines    int     `json:"numGoroutines"`
	SystemCPUPercent float64 `json:"systemCPUPercent"`
	SystemRAMPercent float64 `json:"systemRAMPercent"`
}

func resourceSnapshot() Snapshot {
	var stats runtime.MemStats
	runtime.ReadMemStats(&stats)
	numGoroutines := runtime.NumGoroutine()

	mem := mem.NewExWindows()
	vmem, _ := mem.VirtualMemory()
	usedRAM := vmem.PhysTotal - vmem.PhysAvail
	ramPercent := 100 * float64(usedRAM) / float64(vmem.PhysTotal)
	cpuPercent, _ := cpu.Percent(0, false)

	return Snapshot{
		Time:             time.Now().Unix(),
		GoHeapAlloc:      stats.Alloc,
		GoTotalAlloc:     stats.TotalAlloc,
		GoSysReserved:    stats.Sys,
		GcRounds:         stats.NumGC,
		TotalGCTime:      stats.PauseTotalNs,
		NumGoroutines:    numGoroutines,
		SystemRAMPercent: ramPercent,
		SystemCPUPercent: cpuPercent[0],
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

	fmt.Println("💾 MEMÓRIA GO (HEAP)")
	fmt.Println(strings.Repeat("─", 80))
	printMemoryChart("Heap Alocado", snapshots, func(s Snapshot) float64 {
		return float64(s.GoHeapAlloc) / (1024 * 1024)
	})

	fmt.Println("\n⚡ CPU DO SISTEMA")
	fmt.Println(strings.Repeat("─", 80))
	printPercentChart("Uso Total de CPU", snapshots, func(s Snapshot) float64 {
		return s.SystemCPUPercent
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
		return float64(s.NumGoroutines)
	})

	fmt.Println("\n📈 RESUMO")
	fmt.Println(strings.Repeat("─", 80))

	// Heap do Go
	avgHeap := calculateAverage(snapshots, func(s Snapshot) float64 {
		return float64(s.GoHeapAlloc)
	})
	maxHeap := calculateMax(snapshots, func(s Snapshot) float64 {
		return float64(s.GoHeapAlloc)
	})
	minHeap := calculateMin(snapshots, func(s Snapshot) float64 {
		return float64(s.GoHeapAlloc)
	})

	avgSystemRAM := calculateAverage(snapshots, func(s Snapshot) float64 {
		return s.SystemRAMPercent
	})

	fmt.Printf("Heap Mínimo            : %s\n", formatBytes(uint64(minHeap)))
	fmt.Printf("Heap Médio             : %s\n", formatBytes(uint64(avgHeap)))
	fmt.Printf("Heap Máximo            : %s\n", formatBytes(uint64(maxHeap)))
	fmt.Printf("Heap Final             : %s\n", formatBytes(last.GoHeapAlloc))
	fmt.Printf("RAM do Sistema (Média) : %.2f%%\n\n", avgSystemRAM)

	// CPU do Sistema
	avgSystemCPU := calculateAverage(snapshots, func(s Snapshot) float64 {
		return s.SystemCPUPercent
	})
	maxSystemCPU := calculateMax(snapshots, func(s Snapshot) float64 {
		return s.SystemCPUPercent
	})
	minSystemCPU := calculateMin(snapshots, func(s Snapshot) float64 {
		return s.SystemCPUPercent
	})

	fmt.Printf("CPU Mínima             : %.2f%%\n", minSystemCPU)
	fmt.Printf("CPU Média              : %.2f%%\n", avgSystemCPU)
	fmt.Printf("CPU Máxima             : %.2f%%\n", maxSystemCPU)
	fmt.Printf("CPU Final              : %.2f%%\n\n", last.SystemCPUPercent)

	// Goroutines
	avgGoroutines := calculateAverage(snapshots, func(s Snapshot) float64 {
		return float64(s.NumGoroutines)
	})
	maxGoroutines := calculateMax(snapshots, func(s Snapshot) float64 {
		return float64(s.NumGoroutines)
	})

	fmt.Printf("Goroutines Média       : %.0f\n", avgGoroutines)
	fmt.Printf("Goroutines Máxima      : %.0f\n", maxGoroutines)
	fmt.Printf("Goroutines Final       : %d\n", last.NumGoroutines)

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

func printPercentChart(title string, data []Snapshot, getValue func(Snapshot) float64) {
	if len(data) == 0 {
		return
	}

	const width = 60
	const height = 10

	min := 0.0
	max := 100.0

	fmt.Printf("%s (%%)\n", title)

	for row := height - 1; row >= 0; row-- {
		threshold := min + (max-min)*float64(row+1)/float64(height)
		label := fmt.Sprintf("%5.0f%% │ ", threshold)
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
		fmt.Println("\nAguardando tarefas em andamento finalizarem...")

		retryTimeoutWaitGroup.Wait()
		workersWaitGroup.Wait()
		stopMonitoring()

		fmt.Println("Todas as tarefas finalizadas.")
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
