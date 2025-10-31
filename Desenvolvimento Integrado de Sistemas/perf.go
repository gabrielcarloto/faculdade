package main

import (
	"fmt"
	"runtime"
	"time"
)

func timeTrack(start time.Time, name string) {
	elapsed := time.Since(start)
	fmt.Printf("⏱️  %s: %v\n", name, elapsed)
}

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
