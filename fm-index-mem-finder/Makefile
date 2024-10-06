CC = gcc --std=c18 -g
BIN_DIR = ./bin
DATA_DIR = ./data/
DNA_DATA = ./data/DNA.txt
DNA_BWT_FOR = ./data/DNA_BWT_FOR.txt
DNA_BWT_REV = ./data/DNA_BWT_REV.txt
TEST_DATA = ./data/test.txt
TEST_BWT_FOR = ./data/test_bwt_for.txt
TEST_BWT_REV = ./data/test_bwt_rev.txt

.PHONY: help clean genBWTs mem_finder

help:
	@echo "Options:"
	@echo "    help clean genBWTs"

genBWTs: $(BIN_DIR)/simple_BWT
	@echo "Generating BWT for DNA.txt in forward direction."
	cat $(DNA_DATA) | $(BIN_DIR)/simple_BWT > $(DNA_BWT_FOR)
	@echo "Generating BWT for DNA.txt reversed."
	cat $(DNA_DATA) | rev | $(BIN_DIR)/simple_BWT > $(DNA_BWT_REV)

genTestBWT: $(BIN_DIR)/simple_BWT
	@echo "Generating BWT for test.txt in forward direction."
	cat $(TEST_DATA) | $(BIN_DIR)/simple_BWT > $(TEST_BWT_FOR)
	@echo "Generating BWT for test.txt reversed."
	cat $(TEST_DATA) | rev | $(BIN_DIR)/simple_BWT > $(TEST_BWT_REV)

gdb_mem_finder: $(BIN_DIR)/mem_finder
	gdb --args $(BIN_DIR)/mem_finder $(DNA_BWT_FOR) $(DNA_BWT_REV)

gdb_mem_finder_test: $(BIN_DIR)/mem_finder
	gdb --args $(BIN_DIR)/mem_finder $(TEST_BWT_FOR) $(TEST_BWT_REV)

mem_finder: $(BIN_DIR)/mem_finder
	$(BIN_DIR)/mem_finder $(DNA_BWT_FOR) $(DNA_BWT_REV)

mem_finder_test: $(BIN_DIR)/mem_finder
	$(BIN_DIR)/mem_finder $(TEST_BWT_FOR) $(TEST_BWT_REV)

$(BIN_DIR)/mem_finder: mem_finder.c fm_index.c
	$(CC) mem_finder.c fm_index.c -o $@

$(BIN_DIR)/fm_index: fm_index.c fm_index.h
	$(CC) fm_index.c -o $@

$(BIN_DIR)/simple_BWT: simple_BWT.c
	$(CC) simple_BWT.c -o $@

$(BIN_DIR)/create_bwt: create_bwt.c
	$(CC) create_bwt.c -o $@

clean:
	rm -f $(BIN_DIR)/*
