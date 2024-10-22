CC = gcc
CFLAGS = -Wall -Iinclude -g
BUILD_DIR = build
SRC_DIR = .

TARGET = $(BUILD_DIR)/memFinder_better

# List of source files
SRCS = $(SRC_DIR)/memFinder_better.c ../code/bloom_filter.c ../code/bitvector.c

# List of object files
OBJS = $(BUILD_DIR)/memFinder_better.o $(BUILD_DIR)/bloom_filter.o $(BUILD_DIR)/bitvector.o

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/memFinder_better.o: $(SRC_DIR)/memFinder_better.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/bloom_filter.o: ../code/bloom_filter.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/bitvector.o: ../code/bitvector.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean