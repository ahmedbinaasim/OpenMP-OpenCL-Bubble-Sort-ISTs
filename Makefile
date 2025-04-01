# Compiler and flags
CC = gcc
MPICC = mpicc
CFLAGS = -Wall -Wextra -g -I./include
OMP_FLAGS = -fopenmp

# Libraries
LIBS = -lm

# Source directories
SRC_DIR = src
SEQ_DIR = $(SRC_DIR)/sequential
PAR_DIR = $(SRC_DIR)/parallel
UTIL_DIR = $(SRC_DIR)/utils
INCLUDE_DIR = include
BUILD_DIR = build

# Source files
SEQ_SRC = $(wildcard $(SEQ_DIR)/*.c)
PAR_SRC = $(wildcard $(PAR_DIR)/*.c)
UTIL_SRC = $(wildcard $(UTIL_DIR)/*.c)

# Object files
SEQ_OBJ = $(patsubst $(SEQ_DIR)/%.c,$(BUILD_DIR)/%.o,$(SEQ_SRC))
PAR_OBJ = $(patsubst $(PAR_DIR)/%.c,$(BUILD_DIR)/%.o,$(PAR_SRC))
UTIL_OBJ = $(patsubst $(UTIL_DIR)/%.c,$(BUILD_DIR)/%.o,$(UTIL_SRC))

# Executables
SEQ_EXE = sequential_ist
PAR_EXE = parallel_ist
HYBRID_EXE = hybrid_ist

# Default target
all: directories $(SEQ_EXE) $(PAR_EXE)

# Create build directory
directories:
	mkdir -p $(BUILD_DIR)

# Sequential implementation
$(SEQ_EXE): $(BUILD_DIR)/sequential_main.o $(SEQ_OBJ) $(UTIL_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# MPI implementation
$(PAR_EXE): $(BUILD_DIR)/parallel_main.o $(PAR_OBJ) $(SEQ_OBJ) $(UTIL_OBJ)
	$(MPICC) $(CFLAGS) -o $@ $^ $(LIBS)

# Hybrid MPI+OpenMP implementation
$(HYBRID_EXE): $(BUILD_DIR)/hybrid_main.o $(PAR_OBJ) $(SEQ_OBJ) $(UTIL_OBJ)
	$(MPICC) $(CFLAGS) $(OMP_FLAGS) -o $@ $^ $(LIBS)

# Compile source files
$(BUILD_DIR)/%.o: $(SEQ_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(PAR_DIR)/%.c
	$(MPICC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/%.o: $(UTIL_DIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/sequential_main.o: $(SRC_DIR)/sequential_main.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/parallel_main.o: $(SRC_DIR)/parallel_main.c
	$(MPICC) $(CFLAGS) -c -o $@ $<

$(BUILD_DIR)/hybrid_main.o: $(SRC_DIR)/hybrid_main.c
	$(MPICC) $(CFLAGS) $(OMP_FLAGS) -c -o $@ $<

# Clean
clean:
	rm -rf $(BUILD_DIR) $(SEQ_EXE) $(PAR_EXE) $(HYBRID_EXE)

.PHONY: all directories clean