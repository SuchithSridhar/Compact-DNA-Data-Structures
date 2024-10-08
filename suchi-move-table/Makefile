# Variables
CC = gcc
CFLAGS = -Wall -Iinclude -g
BUILD_DIR = build
SRC_DIR = src

# Source files
FM_MEM_FINDER_SRC = $(SRC_DIR)/fm_mem_finder.c $(SRC_DIR)/fm_index.c
BWT_GENERATOR_SRC = $(SRC_DIR)/bwt_generator.c $(SRC_DIR)/bwt.c $(SRC_DIR)/file_utils.c
MOVE_GENERATOR_SRC = $(SRC_DIR)/move_generator.c $(SRC_DIR)/move_table.c $(SRC_DIR)/file_utils.c
MOVE_OCCR_SRC = $(SRC_DIR)/move_find_occurences.c $(SRC_DIR)/move_table.c $(SRC_DIR)/file_utils.c
MOVE_MEM_FINDER_SRC = $(SRC_DIR)/move_mem_finder.c $(SRC_DIR)/move_table.c $(SRC_DIR)/file_utils.c

# Object files
FM_MEM_FINDER_OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(FM_MEM_FINDER_SRC))
BWT_GENERATOR_OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(BWT_GENERATOR_SRC))
MOVE_GENERATOR_OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(MOVE_GENERATOR_SRC))
MOVE_OCCR_OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(MOVE_OCCR_SRC))
MOVE_MEM_FINDER_OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(MOVE_MEM_FINDER_SRC))

# Executable names
FM_MEM_FINDER_TARGET = $(BUILD_DIR)/fm_mem_finder
BWT_GENERATOR_TARGET = $(BUILD_DIR)/bwt_generator
MOVE_GENERATOR_TARGET = $(BUILD_DIR)/move_generator
MOVE_OCCR_TARGET = $(BUILD_DIR)/move_find_occurences
MOVE_MEM_FINDER_TARGET = $(BUILD_DIR)/move_mem_finder

# Default target
all: $(FM_MEM_FINDER_TARGET) $(BWT_GENERATOR_TARGET) $(MOVE_GENERATOR_TARGET) $(MOVE_OCCR_TARGET) $(MOVE_MEM_FINDER_TARGET)

# Rules to build the executables
$(FM_MEM_FINDER_TARGET): $(FM_MEM_FINDER_OBJS)
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

$(MOVE_MEM_FINDER_TARGET): $(MOVE_MEM_FINDER_OBJS)
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
