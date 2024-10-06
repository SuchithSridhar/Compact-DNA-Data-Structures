#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include "fm_index.h"

long get_file_length(FILE *file) {
    if (!file) return -1;

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    return length;
}

void read_bwt_file(char *bwt, long bwt_length, FILE *file) {
    fread(bwt, 1, bwt_length, file);
    bwt[bwt_length] = '\0';
}

int init_symbols(fm_index *fm) {
    // One 256 array of indices of symbols in fm->symbols
    // if fm->symbols_index[i] == -1 then symbol not present
    fm->symbol_indices = calloc(SYMBOLS_MAX, sizeof(int8_t));
    if (!fm->symbol_indices) return FM_INIT_MEM_ERR;

    for (size_t i = 0; i < SYMBOLS_MAX; i++) fm->symbol_indices[i] = -1;

    fm->symbols_len = 0;

    for (size_t i = 0; i < fm->bwt_length; i++) {
        char c = fm->bwt[i];
        if (fm->symbol_indices[c] == -1 && c != DOLLAR_SIGN) {
            fm->symbol_indices[c] = 1;
            fm->symbols_len++;
        }
    }

    fm->symbols = malloc(fm->symbols_len * sizeof(char));

    int index = 0;
    for (size_t i = 0; i < SYMBOLS_MAX; i++) {
        if (fm->symbol_indices[i] != -1) {
            fm->symbols[index] = (char) i;
            fm->symbol_indices[i] = index;
            index++;
        }
    }

    return FM_INIT_SUCCESS;
}

void init_masks(uint64_t masks[MASKS_COUNT]) {
    int last = MASKS_COUNT - 1;
    masks[last] = ~((uint64_t)0);
    for (int i = last-1; i >= 0; i--) {
        masks[i] = masks[i + 1] & ~((uint64_t)1 << (i + 1));
    }
}

int init_bitvectors(fm_index *fm) {
    long bitvector_size = (fm->bwt_length + (BYTE_SIZE-1)) / BYTE_SIZE;
    long miniheader_size = (fm->bwt_length + (MINIHEADER_SIZE - 1)) / MINIHEADER_SIZE;
    long macroheader_size = (fm->bwt_length + (MACROHEADER_SIZE - 1)) / MACROHEADER_SIZE;

    fm->bitvectors = malloc(sizeof(bitvector) * fm->symbols_len);

    if (!fm->bitvectors) {
        return FM_INIT_MEM_ERR;
    }

    for (size_t i = 0; i < fm->symbols_len; i++) {
        fm->bitvectors[i].bits = calloc(bitvector_size, sizeof(uint8_t));
        fm->bitvectors[i].miniheader = calloc(miniheader_size, sizeof(uint16_t));
        fm->bitvectors[i].macroheader = calloc(macroheader_size, sizeof(uint64_t));

        if (!fm->bitvectors[i].bits || 
            !fm->bitvectors[i].miniheader || 
            !fm->bitvectors[i].macroheader){
            	return FM_INIT_MEM_ERR;
        }
    }

    uint64_t *ranks = calloc(fm->symbols_len, sizeof(uint64_t));
    uint16_t *mini_ranks = calloc(fm->symbols_len, sizeof(uint16_t));

    for (size_t i = 0; i < fm->bwt_length; i++) {
        if (i % MACROHEADER_SIZE == 0) {
            // set all macroheaders
            for (size_t j = 0; j < fm->symbols_len; j++) {
                fm->bitvectors[j].macroheader[i / MACROHEADER_SIZE] = ranks[j];
                mini_ranks[j] = 0;  // reset mini rank since new macro block
            }
        }
        if (i % MINIHEADER_SIZE == 0) {
            // set all miniheaders
            for (size_t j = 0; j < fm->symbols_len; j++) {
                fm->bitvectors[j].miniheader[i / MINIHEADER_SIZE] = mini_ranks[j];
            }
        }

        if (fm->bwt[i] == DOLLAR_SIGN) {
            fm->dollar_position = i;
            continue;
        }

        int8_t index = fm->symbol_indices[fm->bwt[i]];
        if (index == -1) {
            return FM_INIT_UNKNOWN_SYMBOL;
        }

        ranks[index]++;
        mini_ranks[index]++;
        fm->bitvectors[index].bits[i/BYTE_SIZE] |= (1 << (i % BYTE_SIZE));
    }


    fm->symbol_counts = calloc(fm->symbols_len, sizeof(int64_t));
    fm->symbol_cumulative_counts = calloc(fm->symbols_len, sizeof(int64_t));

    if (!fm->symbol_counts || !fm->symbol_cumulative_counts) {
        return FM_INIT_MEM_ERR;
    }

    int64_t total = 0;
    for (size_t i = 0; i < fm->symbols_len; i++) {
        fm->symbol_counts[i] = ranks[i];
        fm->symbol_cumulative_counts[i] = total;
        total += ranks[i];
    }

    free(ranks);
    free(mini_ranks);

    return FM_INIT_SUCCESS;
}

fm_index* fm_index_alloc() {
    fm_index *fm = malloc(sizeof(fm_index));
    if (!fm) return NULL;
    return fm;
}

int fm_index_init(fm_index *fm, char *bwt_filename) {
    FILE *file = fopen(bwt_filename, "r");
    if (!file) return FM_INIT_FILE_ERR;

    fm->bwt_length = get_file_length(file);
    if (fm->bwt_length == -1) return FM_INIT_BWT_LEN_ERR;

    fm->bwt = malloc(fm->bwt_length + 1);
    if (!fm->bwt) return FM_INIT_MEM_ERR;

    read_bwt_file(fm->bwt, fm->bwt_length, file);
    fclose(file);

    init_masks(fm->masks);
    init_symbols(fm);
    init_bitvectors(fm);

    return FM_INIT_SUCCESS;
}


int64_t fm_index_rank(fm_index *fm, char character, int position) {
    if (position < 0) {
        return 0;
    }
    int8_t index = fm->symbol_indices[character];

    if (index == -1) {
        // tried to look for non-existent character
        return -1;
    }

    int64_t sum = 0;

    int64_t mini_value = (int64_t) fm->bitvectors[index].miniheader[position / MINIHEADER_SIZE];
    int64_t macro_value = (int64_t) fm->bitvectors[index].macroheader[position / MACROHEADER_SIZE];

    sum = mini_value + macro_value;

    uint64_t miniblock_chunk = *(uint64_t *) &(fm->bitvectors[index].bits)[(position / MINIHEADER_SIZE) * BYTE_SIZE];

    miniblock_chunk = miniblock_chunk & fm->masks[position % MINIHEADER_SIZE];

    sum += __builtin_popcountll(miniblock_chunk);
	return sum;
}


int64_t fm_index_count_occur(fm_index *fm, char *query) {
    int64_t m = strlen(query);
    int64_t start = 0;
    int64_t end = fm->bwt_length - 1;
    int64_t start_rank = 0;
    int64_t end_rank = 0;

    for (int64_t i = m-1; i >= 0; i--) {
        int8_t index = fm->symbol_indices[query[i]];
        if (index == -1) {
            // symbol not found
            return -1;
        }
        
        start_rank = fm_index_rank(fm, query[i], start-1);
        end_rank = fm_index_rank(fm, query[i], end);

        if (start_rank == -1 || end_rank == -1) {
            // invalid character or some rank error 
            return -1;
        }

        start = fm->symbol_cumulative_counts[index] + start_rank + 1;
        end = fm->symbol_cumulative_counts[index] + end_rank;

        if (start > end) {
            return 0;
        }
    }

    return end - start + 1;
}

void fm_index_destroy(fm_index *fm) {
    if (!fm) return;

    if (fm->bwt) free(fm->bwt);
    if (fm->symbols) free(fm->symbols);
    if (fm->symbol_indices) free(fm->symbol_indices);
    if (fm->symbol_counts) free(fm->symbol_counts);
    if (fm->symbol_cumulative_counts) free(fm->symbol_cumulative_counts);

    for (size_t i = 0; i < fm->symbols_len; i++) {
        if (fm->bitvectors && fm->bitvectors[i].bits) {
            free(fm->bitvectors[i].bits);
        }
        if (fm->bitvectors[i].miniheader) {
            free(fm->bitvectors[i].miniheader);
        }
        if (fm->bitvectors[i].macroheader) {
            free(fm->bitvectors[i].macroheader);
        }
    }

    if (fm->bitvectors) {
        free(fm->bitvectors);
    }

    free(fm);
}
