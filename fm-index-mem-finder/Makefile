CC = gcc --std=c18 -O3
BIN_DIR = ./bin
TEST_DIR = ./test
DATA_DIR = ./data
TEST_PATTERN = $(TEST_DIR)/pattern.txt
DNA_DATA = $(DATA_DIR)/DNA.txt
DNA_BWT_FOR = $(DATA_DIR)/DNA_BWT_FOR.txt
DNA_BWT_REV = $(DATA_DIR)/DNA_BWT_REV.txt

.PHONY: help clean dna_bwts mem_finder create_dirs test_mem_finder

help:
	@echo "Options:"
	@echo "    help dna_bwts mem_finder help_mem_finder test_mem_finder"
	@echo "    note: mem_finder reports MEMs found in the pattern text."
	@echo "          It prints [mem_start, mem_end) within the pattern."

dna_bwts: $(BIN_DIR)/simple_BWT
	@if [ ! -f $(DNA_BWT_FOR) ]; then \
		echo "Generating BWT for DNA.txt in forward direction."; \
		cat $(DNA_DATA) | $(BIN_DIR)/simple_BWT > $(DNA_BWT_FOR); \
	else \
		echo "$(DNA_BWT_FOR) already exists, skipping forward BWT generation."; \
	fi
	@if [ ! -f $(DNA_BWT_REV) ]; then \
		echo "Generating BWT for DNA.txt in reverse direction."; \
		cat $(DNA_DATA) | rev | $(BIN_DIR)/simple_BWT > $(DNA_BWT_REV); \
	else \
		echo "$(DNA_BWT_REV) already exists, skipping reverse BWT generation."; \
	fi

mem_finder: $(BIN_DIR)/mem_finder dna_bwts create_dirs
	$(BIN_DIR)/mem_finder $(DNA_BWT_FOR) $(DNA_BWT_REV)

mem_finder_mvt: $(BIN_DIR)/mem_finder dna_bwts create_dirs
	$(BIN_DIR)/mem_finder $(DNA_BWT_FOR) $(DNA_BWT_REV)

help_mem_finder: $(BIN_DIR)/mem_finder create_dirs
	$(BIN_DIR)/mem_finder

test_mem_finder: $(BIN_DIR)/mem_finder dna_bwts create_dirs
	$(BIN_DIR)/mem_finder $(DNA_BWT_FOR) $(DNA_BWT_REV) < $(TEST_PATTERN)

$(BIN_DIR)/mem_finder: mem_finder.c fm_index.c create_dirs
	$(CC) mem_finder.c fm_index.c -o $@

$(BIN_DIR)/simple_BWT: simple_BWT.c create_dirs
	$(CC) simple_BWT.c -o $@

create_dirs:
	mkdir -p $(BIN_DIR) $(TEST_DIR)

clean:
	rm -f $(BIN_DIR)/*
	rm -f $(DNA_BWT_FOR) $(DNA_BWT_REV)
