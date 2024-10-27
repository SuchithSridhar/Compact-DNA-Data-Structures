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

int main(int argc, char **argv) {
    if (argc != 6) {
        fprintf(stderr,
                "Usage: %s <input text file> <output filename> [k-mer length] "
                "[filter size in bits] [insertion parameter]\n",
                argv[0]);
        fprintf(stderr, "\t* input text file: the source text file to create "
                        "bloom filter for.\n");
        fprintf(stderr,
                "\t* output filename: the file to save the bloom filter to.\n");
        fprintf(stderr, "\t* k-mer length: size of each k-mer tuple.\n");
        fprintf(stderr, "\t* filter size: recommended to be about 10 bits per "
                        "item in set.\n");
        fprintf(stderr,
                "\t* insertion parameter: control ratio of kmers inserted.\n");
        return EXIT_FAILURE;
    }

    Text t = file_utils_read(argv[1]);
    kmer_filter_t kmer_filter;
    init_kmer_struct(&kmer_filter, atol(argv[4]), atoi(argv[3]), atoi(argv[5]));
    kmerf_populate_result_t r = kmerf_populate(&kmer_filter, t);
    int err = kmerf_save_file(&kmer_filter, argv[2]);

    if (err != EXIT_SUCCESS) {
        fprintf(stderr, "Unable to write to file %s\n", argv[2]);
        return EXIT_FAILURE;
    }

    printf("Created a file %s where %zu bits set and %zu tuples inserted.\n",
           argv[2], r.bits_set, r.kmers_inserted);

    return EXIT_SUCCESS;
}
