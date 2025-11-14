package main

import (
	"fmt"
	"os"
	"strconv"
	"strings"

	"github.com/shirou/gopsutil/v4/mem"
)

func getContainerMemoryStats() (uint64, uint64, error) {
	limitPathV2 := "/sys/fs/cgroup/memory.max"
	usagePathV2 := "/sys/fs/cgroup/memory.current"

	if _, err := os.Stat(limitPathV2); err == nil {
		limit, err := readCgroupValue(limitPathV2)
		if err != nil && err.Error() != "is 'max'" {
			return 0, 0, fmt.Errorf("failed to read v2 limit: %w", err)
		}

		if err != nil && err.Error() == "is 'max'" {
			return 0, 0, fmt.Errorf("cgroup v2 limit is 'max'")
		}

		usage, err := readCgroupValue(usagePathV2)
		if err != nil {
			return 0, 0, fmt.Errorf("failed to read v2 usage: %w", err)
		}

		return limit, usage, nil
	}

	limitPathV1 := "/sys/fs/cgroup/memory/memory.limit_in_bytes"
	usagePathV1 := "/sys/fs/cgroup/memory/memory.usage_in_bytes"

	if _, err := os.Stat(limitPathV1); err == nil {
		limit, err := readCgroupValue(limitPathV1)
		if err != nil {
			return 0, 0, fmt.Errorf("failed to read v1 limit: %w", err)
		}

		usage, err := readCgroupValue(usagePathV1)
		if err != nil {
			return 0, 0, fmt.Errorf("failed to read v1 usage: %w", err)
		}

		return limit, usage, nil
	}

	return 0, 0, fmt.Errorf("could not find cgroup memory files (v1 or v2)")
}

func readCgroupValue(path string) (uint64, error) {
	data, err := os.ReadFile(path)
	if err != nil {
		return 0, err
	}

	valStr := strings.TrimSpace(string(data))

	if valStr == "max" {
		return 0, fmt.Errorf("is 'max'")
	}

	val, err := strconv.ParseUint(valStr, 10, 64)
	if err != nil {
		return 0, fmt.Errorf("failed to parse '%s' from %s: %w", valStr, path, err)
	}
	return val, nil
}

type MemoryUsage struct {
	Total       uint64
	Used        uint64
	Available   uint64
	UsedPercent float64
}

func GetMemoryUsage() (*MemoryUsage, error) {
	stats := &MemoryUsage{}

	totalCgroup, usageCgroup, cgroupErr := getContainerMemoryStats()

	if cgroupErr == nil {
		stats.Total = totalCgroup
		stats.Used = usageCgroup
		stats.Available = totalCgroup - usageCgroup
		stats.UsedPercent = (float64(usageCgroup) / float64(totalCgroup)) * 100.0

		return stats, nil
	}

	vmem, vmemErr := mem.VirtualMemory()
	if vmemErr != nil {
		return nil, fmt.Errorf("failed to read host memory: %w", vmemErr)
	}

	stats.Total = vmem.Total
	stats.Used = vmem.Used
	stats.Available = vmem.Available
	stats.UsedPercent = vmem.UsedPercent

	return stats, nil
}
