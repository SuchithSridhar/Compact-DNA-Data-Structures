CC = gcc
CFLAGS = -Wall -Iinclude -g
BUILD_DIR = build
SRC_DIR = .

TARGETS = $(BUILD_DIR)/test_bitvector $(BUILD_DIR)/test_bloom_filter $(BUILD_DIR)/build_kmer_filter $(BUILD_DIR)/kmer_substrings

BITVECTOR_OBJS = $(BUILD_DIR)/bitvector.o $(BUILD_DIR)/test_bitvector.o
BLOOM_FILTER_OBJS = $(BUILD_DIR)/bitvector.o $(BUILD_DIR)/bloom_filter.o $(BUILD_DIR)/test_bloom_filter.o
KMER_FILTER_OBJS = $(BUILD_DIR)/build_kmer_filter.o $(BUILD_DIR)/kmer_filter.o $(BUILD_DIR)/bitvector.o $(BUILD_DIR)/bloom_filter.o $(BUILD_DIR)/file_utils.o
KMER_SUBSTRINGS = $(BUILD_DIR)/kmer_substrings.o $(BUILD_DIR)/kmer_filter.o $(BUILD_DIR)/bitvector.o $(BUILD_DIR)/bloom_filter.o $(BUILD_DIR)/file_utils.o $(BUILD_DIR)/ssvector.o 

all: $(TARGETS)

$(BUILD_DIR)/kmer_substrings: $(KMER_SUBSTRINGS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/build_kmer_filter: $(KMER_FILTER_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/test_bitvector: $(BITVECTOR_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/test_bloom_filter: $(BLOOM_FILTER_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)/*

.PHONY: all clean
