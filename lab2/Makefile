CC = gcc
CFLAGS = -I. -lm

HDF5_FLAGS = -I/usr/include/hdf5/serial -L/usr/lib/x86_64-linux-gnu/hdf5/serial -lhdf5
COMMON_HDRS = ./utils/data_utils.h ./kernel/conv.h ./kernel/matrix_ops.h ./kernel/linear.h ./kernel/functional.h ./kernel/nn.h ./kernel/attention.h
COMMON_SRC = ./utils/data_utils.c ./kernel/conv.c ./kernel/functional.c ./kernel/matrix_ops.c ./kernel/linear.c ./kernel/nn.c ./kernel/attention.c

# Unity test framework
UNITY_FILES = ./tests/unity/unity.c
TEST_FILES = ./tests/all_tests.c ./tests/test_attention.c ./tests/test_conv.c ./tests/test_functional.c ./tests/test_linear.c ./tests/test_matrix_ops.c
TEST_EXECUTABLES = $(patsubst %.c,%,$(TEST_FILES))

# Performance
MATMUL_TARGETS = matmul_naive matmul_blocking matmul_blocking_ijk
SIZE ?= 1024
REPS ?= 3
LEVEL ?= 1
# Tile sizes for matmul_blocking, e.g. make matmul_blocking BI=64 BJ=256 BK=32
BLOCK_FLAGS = $(if $(BI),-DBLOCK_SIZE_I=$(BI)) $(if $(BJ),-DBLOCK_SIZE_J=$(BJ)) $(if $(BK),-DBLOCK_SIZE_K=$(BK))

BINS = lab2

.PHONY: all
all: $(BINS)

.PHONY: lab2
lab2: lab2.c $(COMMON_HDRS)
	$(CC) -o $@ lab2.c $(COMMON_SRC) $(CFLAGS) $(HDF5_FLAGS)

.PHONY: test
test: all_tests

.PHONY: all_tests
all_tests: 
	$(CC) -o tests/$@ $(TEST_FILES) $(UNITY_FILES) $(COMMON_SRC) $(CFLAGS) $(HDF5_FLAGS) 
	./tests/$@

.PHONY : $(MATMUL_TARGETS)
$(MATMUL_TARGETS):
	$(CC) -o $@ ./perf/$@.c $(COMMON_SRC) $(CFLAGS) $(BLOCK_FLAGS) $(HDF5_FLAGS)
	# Pin the program to CPU 0 (= socket 0, core 0 on the ece5755 node, no SMT)
	# and collect Top-down counters for that core only
	python3 /classes/ece5755/pmu-tools/toplev.py --core S0-C0 -l$(LEVEL) -v --no-desc --force-cpu spr taskset -c 0 ./$@ $(SIZE) $(REPS)

.PHONY: clean
clean:
	rm -f $(BINS) $(TEST_EXECUTABLES) $(GRADING_EXECUTABLES) $(GRADING_TESTS_OUTPUT) $(MATMUL_TARGETS)