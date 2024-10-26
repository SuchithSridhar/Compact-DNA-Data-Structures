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
    }

    Text t = file_utils_read(argv[1]);
    kmer_filter_t kmer_filter = {
        .bloom = bf_create(atol(argv[4]), hash_funcs, HASH_FUNCTION_COUNT),
        .kmer_size = atoi(argv[3]),
        .filter_size = atol(argv[4]),
        .five_to_pow_k = 1,
        .insertion_hash = insertion_param_hash,
        .insertion_param = atoi(argv[5])};

    for (int i = 0; i < kmer_filter.kmer_size; i++) {
        kmer_filter.five_to_pow_k *= 5;
    }

    return EXIT_SUCCESS;
}
