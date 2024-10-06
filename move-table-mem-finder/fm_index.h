#ifndef __FM_INDEX_H
#define __FM_INDEX_H

#include <stdint.h>

#define MASKS_COUNT 64
#define SYMBOLS_MAX 256
#define MINIHEADER_SIZE 64
#define MACROHEADER_SIZE 65536
#define DOLLAR_SIGN '$'
#define BYTE_SIZE 8

#define FM_INIT_SUCCESS 0
#define FM_INIT_FILE_ERR 1
#define FM_INIT_BWT_LEN_ERR 2
#define FM_INIT_MEM_ERR 3
#define FM_INIT_UNKNOWN_SYMBOL 4

typedef struct {
    uint8_t *bits;
    uint16_t *miniheader;
    uint64_t *macroheader;
} bitvector;

typedef struct {
    char *bwt;
    char *symbols;
    int8_t *symbol_indices;
    int64_t *symbol_counts;
    int64_t *symbol_cumulative_counts;
    bitvector *bitvectors;
    uint64_t masks[MASKS_COUNT];
    int64_t dollar_position;
    int64_t bwt_length;
    uint8_t symbols_len;
} fm_index;

/**
 * Allocate memory for an FM index.
 */
fm_index* fm_index_alloc();

/**
 * Initialize an FM index with a given BWT.
 */
int fm_index_init(fm_index *fm, char *bwt_filename);

/**
 * Destroy an FM index and free all allocated memory.
 */
void fm_index_destroy(fm_index *fm);

/**
 * Find the rank of a given symbol at a given position in an FM index.
 */
int64_t fm_index_rank(fm_index *fm, char symbol, int position);

/**
 * Find the occurrences of given query in an FM index.
 */
int64_t fm_index_count(fm_index *fm, char *query);

#endif
