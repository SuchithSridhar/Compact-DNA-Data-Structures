#ifndef KMER_MEM
#include "file_utils.h"
#include "kmer_filter.h"
#include <stdlib.h>

typedef struct row {
    char head;
    unsigned char length;
    int pointer;
    unsigned char offset;
} row_t;

typedef struct move_table {
    row_t *table;
    int r;
    int n;
} move_table_t;

move_table_t read_mvt(char *);
int forward_backward(row_t *, int, int64_t, char *, size_t, int8_t);
int find_mems(row_t *, int, row_t *, int, char *, size_t);
void find_substrings(Text, kmer_filter_t *, int, row_t *, row_t *);

#endif // !KMER_MEM
