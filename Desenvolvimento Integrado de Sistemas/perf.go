package main

import (
	"encoding/json"
	"fmt"
	"os"
	"os/signal"
	"runtime"
	"syscall"
	"time"

	"github.com/shirou/gopsutil/v4/cpu"
)

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
	SystemUsedRAM    uint64  `json:"systemUsedRAM"`
}

func resourceSnapshot() Snapshot {
	var stats runtime.MemStats
	runtime.ReadMemStats(&stats)
	numGoroutines := runtime.NumGoroutine()

	vmem, _ := GetMemoryUsage()
	usedRAM := vmem.Used
	ramPercent := 100 * float64(usedRAM) / float64(vmem.Total)
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
		SystemUsedRAM:    usedRAM,
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

		if err := exportReportJSON("./out/performance_report.json"); err != nil {
			fmt.Printf("Erro ao exportar JSON: %v\n", err)
		} else {
			fmt.Println("Relatório JSON exportado: ./out/performance_report.json")
		}

		os.Exit(0)
	}()
}
