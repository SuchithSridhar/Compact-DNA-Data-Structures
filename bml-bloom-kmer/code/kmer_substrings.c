#include "file_utils.h"
#include "kmer_filter.h"
#include "kmer_mems.h"
#include "string_utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATTERN_LENGTH 1000000
#define MIN_MEM_LENGTH 10

#define FORWARD 1
#define BACKWARD -1

#define SLTEE(s_run, s_offset, e_run, e_offset)                                \
    ((s_run) < (e_run) || ((s_run) == (e_run) && (s_offset) <= (e_offset)))

/**
 * @brief Loads a move table from a binary move table file
 *
 * @param[in] filename name of the binary move table file
 * @return A move table
 *
 */
move_table_t read_mvt(char *filename) {
    FILE *move = fopen(filename, "rb");

    int n;
    fread(&n, sizeof(int), 1, move);
    fprintf(stderr, "read n = %i\n", n);

    int r;
    fread(&r, sizeof(int), 1, move);
    fprintf(stderr, "read r = %i\n", r);

    row_t *table = (row_t *)malloc(r * sizeof(row_t));

    for (int i = 0; i < r; i++) {
        fread(&table[i].head, 1, 1, move);
        fread(&table[i].length, 1, 1, move);
        fread(&table[i].pointer, sizeof(int), 1, move);
        fread(&table[i].offset, 1, 1, move);
    }

    fclose(move);

    fprintf(stderr, "loaded table\n");

    return (move_table_t){.table = table, .r = r, .n = n};
}

/**
 * @brief Finds the MEMs for a given pattern using the forward backwards
 * algorithm
 *
 *
 * @param[in] mtb move table
 * @param[in] mt_length number of rows in the move table
 * @param[in] start
 * @param[in] pattern
 * @param[in] pattern_size length of the pattern
 * @param[in] direction If +1, we step forwards in the pattern, and if -1, we
 *            step backwards
 *
 * @return A move table
 *
 */
int forward_backward(row_t *mtb, int mt_length, int64_t start, char *pattern,
                     size_t pattern_size, int8_t direction) {

    int64_t s_run = 0;
    int16_t s_offset = 0;
    int64_t e_run = mt_length - 1;
    int16_t e_offset = mtb[e_run].length - 1;

    int pi = start;
    int counter = 0;

    while (SLTEE(s_run, s_offset, e_run, e_offset) && pi < pattern_size &&
           pi >= 0) {

        while (s_run < mt_length && mtb[s_run].head != pattern[pi]) {
            s_run++;
            s_offset = 0;
        }

        while (e_run >= 0 && mtb[e_run].head != pattern[pi]) {
            e_run--;
            e_offset = mtb[e_run].length - 1;
        }

        if (!SLTEE(s_run, s_offset, e_run, e_offset)) {
            break;
        }

        s_offset += mtb[s_run].offset;
        s_run = mtb[s_run].pointer;

        e_offset += mtb[e_run].offset;
        e_run = mtb[e_run].pointer;

        while (s_offset >= mtb[s_run].length) {
            s_offset -= mtb[s_run].length;
            s_run++;
        }

        while (e_offset >= (int)mtb[e_run].length) {
            e_offset -= (int)mtb[e_run].length;
            e_run++;
        }

        pi += direction;
        counter++;
    }

    return counter;
}

/**
 * @brief Find maximal exact matches (MEMs) for a given pattern within a text.
 *
 * @param[in] mt_straight move table of the original text
 * @param[in] mt_straight_length length of the move table of the original text
 * @param[in] mt_reversed move table of the reversed text
 * @param[in] mt_reversed_length length of the move table of the reversed text
 * @param[in] pattern
 * @param[in] pattern_size length of the pattern
 * @return the number of MEMs found for the given pattern
 *
 */
int find_mems(row_t *mt_straight, int mt_straight_length, row_t *mt_reversed,
              int mt_reversed_length, char *pattern, size_t pattern_size) {
    int64_t mem_start = 0;
    int64_t mem_end = 0;
    int64_t mem_count = 0;

    while (mem_end <= pattern_size - 1) {
        int64_t steps_fw =
            forward_backward(mt_reversed, mt_reversed_length, mem_start,
                             pattern, pattern_size, FORWARD);
        mem_end = mem_start + steps_fw;

        if (steps_fw >= MIN_MEM_LENGTH) {
            mem_count++;
            printf("MEM (start=%lld, end=%lld, len=%lld)\n", mem_start, mem_end,
                   steps_fw);
        }

        if (mem_end >= pattern_size) {
            break;
        }

        int64_t steps_bw =
            forward_backward(mt_straight, mt_straight_length, mem_end, pattern,
                             pattern_size, BACKWARD);
        int64_t new_mem_start = mem_end - steps_bw + 1;

        mem_start = new_mem_start;
        printf("%lld %lld\n", mem_start, mem_end);
    }

    return mem_count;
}

/**
 * @brief Finds the Maximal Exact Matches (MEMs) of a text given a pattern using
 * a kmer bloom filter.
 *
 * Finds maximal substrings of the pattern that occurs in the text and finds the
 * MEMs in each of the substrings.
 *
 * @param[in] pattern the pattern used to match again the text
 * @param[in] kmer_filter the kmer filter
 * @param[in] min_mem_length minimum length of the MEMs to be found
 * @param[in] mvt_straight move table of the original text
 * @param[in] mvt_reversed move table of the reversed text
 *
 */
void find_kmer_mems(Text pattern, kmer_filter_t *kmer_filter,
                    int min_mem_length, row_t *mvt_straight,
                    row_t *mvt_reversed) {
    kmer_filter_t *kf = kmer_filter;

    size_t start_substring = 0;
    size_t end_substring = 0;

    for (size_t i = 0; i < pattern.len - kf->kmer_size; i++) {
        kmer_int_t kmer = kmerf_as_int(kf, pattern.T, i);
        if (kmerf_should_contain(kf, kmer) && !kmerf_query(kf, kmer)) {
            // a kmer not found in bloom filter
            end_substring = i + kf->kmer_size;
            if (end_substring - start_substring >= min_mem_length) {
                char *substring =
                    get_substring(pattern.T, start_substring, end_substring);
                find_mems(mvt_straight, mvt_straight->length, mvt_reversed,
                          mvt_reversed->length, substring, strlen(substring));
                free(substring);
            }

            start_substring = i + 1;
        }
    }

    if (pattern.len - start_substring >= min_mem_length) {
        char *substring =
            get_substring(pattern.T, start_substring, end_substring);
        find_mems(mvt_straight, mvt_straight->length, mvt_reversed,
                  mvt_reversed->length, substring, strlen(substring));
        free(substring);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 6) {
        fprintf(stderr,
                "usage: %s <pattern file> <move table file> <reversed move "
                "table file> [kmer filter file] "
                "<min mem length>\n",
                argv[0]);
        return EXIT_FAILURE;
    }

    char *pattern_file = argv[1];
    char *kmer_filter_file = argv[4];
    char *min_mem_len_str = argv[5];

    move_table_t table_straight = read_mvt(argv[2]);
    move_table_t table_reversed = read_mvt(argv[3]);

    kmer_filter_t kmer_filter;
    Text pattern = file_utils_read(pattern_file);
    int min_mem_length = atoi(min_mem_len_str);
    kmerf_load_file(&kmer_filter, kmer_filter_file);

    find_kmer_mems(pattern, &kmer_filter, min_mem_length, table_straight.table,
                   table_reversed.table);

    return EXIT_SUCCESS;
}
