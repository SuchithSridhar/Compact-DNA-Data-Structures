#include "bloom_filter.h"
#include "bitvector.h"
#include <assert.h>
#include <stdlib.h>

bloom_filter_t *bf_create(size_t bits, hash_function_t *hash_funcs,
                          size_t hash_funcs_count) {

    assert(hash_funcs != NULL && hash_funcs_count >= 1);

    bloom_filter_t *bf = malloc(sizeof(bloom_filter_t));

    if (bf == NULL) {
        return NULL;
    }

    bf->bv = bv_create(bits);

    if (bf->bv == NULL) {
        free(bf);
        return NULL;
    }

    bf->hash_functions_count = hash_funcs_count;
    bf->hash_functions = hash_funcs;

    return bf;
}

void bf_destroy(bloom_filter_t *bf) {
    assert(bf != NULL && bf->bv != NULL);

    bv_destroy(bf->bv);
    free(bf);
}

uint64_t bf_hash(hash_function_t *hf, int64_t value) {
    assert(hf != NULL);
    return ((hf->A * value) + hf->B) % hf->P;
}

void bf_insert(bloom_filter_t *bf, int64_t value) {
    assert(bf != NULL);

    for (int i = 0; i < bf->hash_functions_count; i++) {
        u_int64_t hash = bf_hash(&bf->hash_functions[i], value);
        bv_set_bit(bf->bv, hash % bf->bv->bits);
    }
}

bool bf_query(bloom_filter_t *bf, int64_t value) {
    assert(bf != NULL);

    for (int i = 0; i < bf->hash_functions_count; i++) {
        u_int64_t hash = bf_hash(&bf->hash_functions[i], value);
        u_int8_t bit = bv_get_bit(bf->bv, hash % bf->bv->bits);
        if (bit == 0) {
            return false;
        }
    }
    return true;
}
