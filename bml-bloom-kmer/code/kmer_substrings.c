#include "file_utils.h"
#include "kmer_filter.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
// TODO: Move to this to use suchi's move table
typedef struct {
    char head;
    unsigned char length;
    int pointer;
    unsigned char offset;
} row_t;

typedef struct {
    row_t *table;
    int n;
    int r;
} move_table_t;
*/

/**
move_table_t *load_move_table(char *table_filename) {
    FILE *file = fopen(table_filename, "rb");

    if (file == NULL) {
        return NULL;
    }

    move_table_t *table = malloc(sizeof(move_table_t));

    fread(&table->n, sizeof(int), 1, file);
    fread(&table->r, sizeof(int), 1, file);

    table->table = malloc(sizeof(row_t) * table->r);

    for (int i = 0; i < table->r; i++) {
        fread(&table->table[i].head, sizeof(int8_t), 1, file);
        fread(&table->table[i].length, sizeof(int8_t), 1, file);
        fread(&table->table[i].pointer, sizeof(int32_t), 1, file);
        fread(&table->table[i].offset, sizeof(int8_t), 1, file);
    }

    fclose(file);

    return table;
}
*/

void range_print_string(char *string, int start, int end) {
    char tmp = string[end];
    string[end] = '\0';
    fprintf(stdout, "%s\n", &string[start]);
    string[end] = tmp;
}

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
