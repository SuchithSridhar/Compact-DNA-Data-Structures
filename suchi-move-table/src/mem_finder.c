#include "fm_index.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_PATTERN_LENGTH 1000000
#define MIN_MEM_LENGTH 40

#define FORWARD 1
#define BACKWARD -1

// direction = 1 -> right (forward)
// direction = -1 -> left (backward)
// I'm going to assume you gave me the right fm index
// returns the steps I was able to walk
int forward_backward(fm_index *fm, int64_t start, char *pattern,
                     size_t pattern_size, int8_t direction) {
    int64_t s = 0;
    int64_t e = fm->bwt_length - 1;

    int64_t *scc = fm->symbol_cumulative_counts;

    int pi = start;
    int counter = 0;

    while (s <= e && pi < pattern_size && pi >= 0) {
        int64_t si = fm->symbol_indices[(int)pattern[pi]];

        int64_t rank_s = fm_index_rank(fm, pattern[pi], s - 1);
        int64_t rank_e = fm_index_rank(fm, pattern[pi], e);
        s = scc[si] + rank_s + 1;
        e = scc[si] + rank_e;

        if (s > e)
            break;

        pi += direction;
        counter++;
    }

    return counter;
}

/**
 * Find maximal exact matches (MEMs) for a given pattern within a text.
 */
int find_mems(fm_index *fm_index_straight, fm_index *fm_index_reversed,
              char *pattern, size_t pattern_size) {
    int64_t mem_start = 0;
    int64_t mem_end = 0;

    while (mem_end <= pattern_size - 1) {
        int64_t steps_fw = forward_backward(fm_index_reversed, mem_start,
                                            pattern, pattern_size, FORWARD);
        mem_end = mem_start + steps_fw;

        if (steps_fw >= MIN_MEM_LENGTH) {
            printf("MEM (start=%ld, end=%ld, len=%ld)\n", mem_start, mem_end,
                   steps_fw);
        }

        if (mem_end >= pattern_size) {
            break;
        }

        int64_t steps_bw = forward_backward(fm_index_straight, mem_end, pattern,
                                            pattern_size, BACKWARD);
        int64_t new_mem_start = mem_end - steps_bw + 1;

        mem_start = new_mem_start;
    }
    return 0;
}

int main(int argc, char **argv) {

    if (argc != 3) {
        fprintf(stderr, "\nUsage: %s <BWT file> <Reversed BWT file>\n",
                argv[0]);
        fprintf(
            stderr,
            "\nThe MEM finder reports MEMs found within the pattern by \n"
            "reporting the starting index, ending index, and the length of \n"
            "the MEM within the pattern. The starting index is inclusive and \n"
            "ending index is exclusive: [mem_start, mem_end).\n\n");
        return EXIT_FAILURE;
    }

    // Load FM-index for the original text
    fm_index *fm = fm_index_alloc();
    if (fm_index_init(fm, argv[1]) != FM_INIT_SUCCESS) {
        fprintf(stderr, "Failed to initialize FM-index from %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    // Load FM-index for the reversed text
    fm_index *rev_fm = fm_index_alloc();
    if (fm_index_init(rev_fm, argv[2]) != FM_INIT_SUCCESS) {
        fprintf(stderr, "Failed to initialize reverse FM-index from %s\n",
                argv[2]);
        fm_index_destroy(fm);
        return EXIT_FAILURE;
    }

    // Read the pattern from stdin
    char *pattern = malloc(MAX_PATTERN_LENGTH * sizeof(char));
    if (!pattern) {
        fprintf(stderr, "Failed to allocate memory for pattern\n");
        fm_index_destroy(fm);
        fm_index_destroy(rev_fm);
        return EXIT_FAILURE;
    }

    size_t read_len = fread(pattern, sizeof(char), MAX_PATTERN_LENGTH, stdin);
    if (read_len == 0) {
        fprintf(stderr, "No pattern data read from stdin\n");
        free(pattern);
        fm_index_destroy(fm);
        fm_index_destroy(rev_fm);
        return EXIT_FAILURE;
    }

    find_mems(fm, rev_fm, pattern, read_len);

    free(pattern);
    fm_index_destroy(fm);
    fm_index_destroy(rev_fm);

    return EXIT_SUCCESS;
}
