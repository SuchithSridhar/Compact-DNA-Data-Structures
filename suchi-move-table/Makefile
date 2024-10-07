# Variables
CC = gcc
CFLAGS = -Wall -Iinclude -g
BUILD_DIR = build
SRC_DIR = src

# Source files
MEM_FINDER_SRC = $(SRC_DIR)/mem_finder.c $(SRC_DIR)/fm_index.c
BWT_GENERATOR_SRC = $(SRC_DIR)/bwt_generator.c $(SRC_DIR)/bwt.c $(SRC_DIR)/file_utils.c
MOVE_GENERATOR_SRC = $(SRC_DIR)/move_generator.c $(SRC_DIR)/move_table.c $(SRC_DIR)/file_utils.c
MOVE_OCCR_SRC = $(SRC_DIR)/move_find_occurences.c $(SRC_DIR)/move_table.c $(SRC_DIR)/file_utils.c

# Object files
MEM_FINDER_OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(MEM_FINDER_SRC))
BWT_GENERATOR_OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(BWT_GENERATOR_SRC))
MOVE_GENERATOR_OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(MOVE_GENERATOR_SRC))
MOVE_OCCR_OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(MOVE_OCCR_SRC))

# Executable names
MEM_FINDER_TARGET = $(BUILD_DIR)/mem_finder
BWT_GENERATOR_TARGET = $(BUILD_DIR)/bwt_generator
MOVE_GENERATOR_TARGET = $(BUILD_DIR)/move_generator
MOVE_OCCR_TARGET = $(BUILD_DIR)/move_find_occurences

# Default target
all: $(MEM_FINDER_TARGET) $(BWT_GENERATOR_TARGET) $(MOVE_GENERATOR_TARGET) $(MOVE_OCCR_TARGET)

# Rules to build the executables
$(MEM_FINDER_TARGET): $(MEM_FINDER_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BWT_GENERATOR_TARGET): $(BWT_GENERATOR_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(MOVE_GENERATOR_TARGET): $(MOVE_GENERATOR_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(MOVE_OCCR_TARGET): $(MOVE_OCCR_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Compile object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Utility targets
clean:
	rm -rf $(BUILD_DIR)/*

# Phony targets
.PHONY: all clean
