#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H

#include "bitvector.h"
#include <stdbool.h>
#include <stdlib.h>

// try to store this on stack
// for optimization
// hash: [(A * v) + B] % P
typedef struct {
    size_t A;
    size_t B;
    size_t P;
} hash_function_t;

typedef struct {
    bitvector_t *bv;
    hash_function_t *hash_functions;
    size_t hash_functions_count;
} bloom_filter_t;

bloom_filter_t *bf_create(size_t bits, hash_function_t *hash_funcs,
                          size_t hash_funcs_count);
void bf_destroy(bloom_filter_t *bf);
uint64_t bf_hash(hash_function_t *hf, int64_t value);
void bf_insert(bloom_filter_t *bf, int64_t value);
bool bf_query(bloom_filter_t *bf, int64_t value);

#endif
