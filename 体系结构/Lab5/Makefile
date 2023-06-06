CC := gcc
CFLAGS := -O2 -mavx -mavx2 -mfma -msse -msse2 -msse3

NVCC := nvcc
NVCCFLAGS := -O2

SRC_DIR := src_cpu
SRCS := $(wildcard $(SRC_DIR)/*.c)
TARGETS := $(notdir $(SRCS:%.c=%))

GPU_SRC_DIR := src_gpu
GPU_SRCS := $(wildcard $(GPU_SRC_DIR)/*.cu)
TARGETS += $(notdir $(GPU_SRCS:%.cu=%))
SRCS += $(GPU_SRCS)

.PHONY: all clean

all: $(TARGETS)

%: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $< -o $@

%: $(GPU_SRC_DIR)/%.cu
	$(NVCC) $(NVCCFLAGS) $< -o $@

clean:
	rm -f $(TARGETS)
