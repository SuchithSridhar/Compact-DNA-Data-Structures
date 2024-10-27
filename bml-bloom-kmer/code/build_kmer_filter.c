#include "bloom_filter.h"
#include "file_utils.h"
#include "kmer_filter.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define HASH_FUNCTION_COUNT 6

hash_function_t hash_funcs[HASH_FUNCTION_COUNT] = {
    {.A = 59515879, .B = 95578445, .P = 56887169},
    {.A = 65835776, .B = 22503992, .P = 34543247},
    {.A = 63372674, .B = 9287645, .P = 31428521},
    {.A = 58184635, .B = 49547574, .P = 64362491},
    {.A = 79157700, .B = 94338060, .P = 15976133},
    {.A = 88994189, .B = 67949655, .P = 63991913}};

// This hash function along with insertion parameter is used to control the
// number of kmers inserted into the filter
hash_function_t insertion_param_hash = {
    .A = 94607073, .B = 54204618, .P = 55586519};

void init_kmer_struct(kmer_filter_t *kf, size_t bv_size, int kmer_size,
                      int ins_param) {
    kf->bloom = bf_create(bv_size, hash_funcs, HASH_FUNCTION_COUNT);
    kf->kmer_size = kmer_size;
    kf->filter_size = bv_size;
    kf->_five_to_pow_km1 = 1;
    kf->_cache_source_string = NULL;
    kf->insertion_hash = insertion_param_hash;
    kf->insertion_param = ins_param;

    for (int i = 0; i < kmer_size - 1; i++) {
        kf->_five_to_pow_km1 *= 5;
    }
}

kmer_filter_t* build_kmer_filter(const char *input_file, const char *output_file, int kmer_length, size_t filter_size, int insertion_param) {
    Text t = file_utils_read(input_file);
    kmer_filter_t kmer_filter;
    init_kmer_struct(&kmer_filter, filter_size, kmer_length, insertion_param);
    kmerf_populate_result_t r = kmerf_populate(&kmer_filter, t);
    int err = kmerf_save_file(&kmer_filter, output_file);

    if (err != EXIT_SUCCESS) {
        fprintf(stderr, "Unable to write to file %s\n", output_file);
        exit(EXIT_FAILURE);
    }

    printf("Created a file %s where %zu bits set and %zu tuples inserted.\n",
           output_file, r.bits_set, r.kmers_inserted);
    
    return &kmer_filter;
}
