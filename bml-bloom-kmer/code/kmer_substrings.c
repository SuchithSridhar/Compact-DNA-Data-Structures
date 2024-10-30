#include "file_utils.h"
#include "kmer_filter.h"
#include "string_utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void find_substrings(Text pattern, kmer_filter_t *kmer_filter,
                     int min_mem_length) {
    kmer_filter_t *kf = kmer_filter;

    size_t start_substring = 0;
    size_t end_substring = 0;

    for (size_t i = 0; i < pattern.len - kf->kmer_size; i++) {
        kmer_int_t kmer = kmerf_as_int(kf, pattern.T, i);
        if (kmerf_should_contain(kf, kmer) && !kmerf_query(kf, kmer)) {
            // a kmer not found in bloom filter
            end_substring = i + kf->kmer_size;
            if (end_substring - start_substring >= min_mem_length) {
                range_print_string(pattern.T, start_substring, end_substring);
            }

            start_substring = i + 1;
        }
    }

    if (pattern.len - start_substring >= min_mem_length) {
        range_print_string(pattern.T, start_substring, pattern.len);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(
            stderr,
            "usage: %s <pattern file> [kmer filter file] <min mem length>\n",
            argv[0]);
        return EXIT_FAILURE;
    }

    char *pattern_file = argv[1];
    char *kmer_filter_file = argv[2];
    char *min_mem_len_str = argv[3];

    kmer_filter_t kmer_filter;
    Text pattern = file_utils_read(pattern_file);
    int min_mem_length = atoi(min_mem_len_str);
    kmerf_load_file(&kmer_filter, kmer_filter_file);

    find_substrings(pattern, &kmer_filter, min_mem_length);

    return EXIT_SUCCESS;
}
