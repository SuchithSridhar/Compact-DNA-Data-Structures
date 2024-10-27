CC = gcc
CFLAGS = -Wall -Iinclude -g
BUILD_DIR = build
SRC_DIR = .

TARGETS = $(BUILD_DIR)/test_bitvector $(BUILD_DIR)/test_bloom_filter $(BUILD_DIR)/memFinder

BITVECTOR_OBJS = $(BUILD_DIR)/bitvector.o $(BUILD_DIR)/test_bitvector.o
BLOOM_FILTER_OBJS = $(BUILD_DIR)/bitvector.o $(BUILD_DIR)/bloom_filter.o $(BUILD_DIR)/test_bloom_filter.o
MEM_FINDER_OBJS = $(BUILD_DIR)/kmer_filter.o $(BUILD_DIR)/bitvector.o $(BUILD_DIR)/bloom_filter.o $(BUILD_DIR)/file_utils.o $(BUILD_DIR)/memFinder.o $(BUILD_DIR)/memFinder.o
all: $(TARGETS)

$(BUILD_DIR)/test_bitvector: $(BITVECTOR_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/test_bloom_filter: $(BLOOM_FILTER_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/memFinder: $(MEM_FINDER_OBJS) 
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)/*

.PHONY: all clean
