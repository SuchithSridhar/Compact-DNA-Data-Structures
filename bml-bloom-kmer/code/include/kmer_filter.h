#ifndef _KMER_FILTER_H
#define _KMER_FILTER_H

#include "bloom_filter.h"
#include "file_utils.h"
#include <stdint.h>

typedef uint64_t kmer_int_t;

// fields that start with _ are internal use fields
typedef struct {
    bloom_filter_t *bloom;
    size_t filter_size;
    hash_function_t insertion_hash;
    // NOTE: Assumption of AGCT
    size_t _five_to_pow_km1;
    kmer_int_t _cache_kmer;
    void *_cache_source_string;
    int _cache_window_start;
    int kmer_size;
    int insertion_param;
} kmer_filter_t;

typedef struct {
    size_t kmers_inserted;
    size_t bits_set;
} kmerf_populate_result_t;

/**
 * Populate the given filter and then return the number of
 * tuples inserted into it.
 */
kmerf_populate_result_t kmerf_populate(kmer_filter_t *kf, Text T);

/**
 * Checks if a given kmer should be contained in the kmer filter.
 * This is based on the insertion parameter and the hash function.
 */
bool kmerf_should_contain(kmer_filter_t *kf, kmer_int_t kmer);

/**
 * Checks if a kmer is in the filter.
 */
bool kmerf_query(kmer_filter_t *kf, kmer_int_t kmer);

/**
 * Adds a kmer to the filter. Returns the number of bits set.
 */
int32_t kmerf_add(kmer_filter_t *kf, kmer_int_t kmer);

/**
 * Get the integer represenatation of a kmer.
 * start is the start of the kmer, end is implicit from
 * kf->kmer_size to the right.
 * You can provide it any string str and any range within it.
 * However, if you provide the same str twice in a row, the function
 * does some optimization to reduce the number of operations.
 */
kmer_int_t kmerf_as_int(kmer_filter_t *kf, char *str, size_t start);

/**
 * Read a kmer_filter from a binary file.
 */
int kmerf_load_file(kmer_filter_t *kf, char *filename);

/**
 * Write a kmer_filter to a binary file.
 */
int kmerf_save_file(kmer_filter_t *kf, char *filename);

#endif
