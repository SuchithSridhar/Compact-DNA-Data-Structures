#include "string_utils.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define FORWARD 1
#define BACKWARD -1

#define SLTEE(s_run, s_offset, e_run, e_offset)                                \
    ((s_run) < (e_run) || ((s_run) == (e_run) && (s_offset) <= (e_offset)))

typedef struct row {
    char head;
    unsigned char length;
    int pointer;
    unsigned char offset;
} move_table_t;

typedef struct props {
    move_table_t *table;
    int r;
    int n;
} props;

props read_mvt(char *filename) {
    FILE *move = fopen(filename, "rb");

    int n;
    fread(&n, sizeof(int), 1, move);
    fprintf(stderr, "read n = %i\n", n);

    int r;
    fread(&r, sizeof(int), 1, move);
    fprintf(stderr, "read r = %i\n", r);

    move_table_t *table = (move_table_t *)malloc(r * sizeof(move_table_t));

    for (int i = 0; i < r; i++) {
        fread(&table[i].head, 1, 1, move);
        fread(&table[i].length, 1, 1, move);
        fread(&table[i].pointer, sizeof(int), 1, move);
        fread(&table[i].offset, 1, 1, move);
    }

    fclose(move);

    fprintf(stderr, "loaded table\n");

    return (props){.table = table, .r = r, .n = n};
}

// direction = 1 -> right (forward)
// direction = -1 -> left (backward)
// I'm going to assume you gave me the right fm index
// returns the steps I was able to walk
int forward_backward(move_table_t *mtb, int mt_length, int64_t start,
                     char *pattern, size_t pattern_size, int8_t direction) {

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
 * Find maximal exact matches (MEMs) for a given pattern within a text.
 */
int find_mems(move_table_t *mt_straight, int mt_straight_length,
              move_table_t *mt_reversed, int mt_reversed_length, char *pattern,
              size_t pattern_size, int min_mem_len) {
    int64_t mem_start = 0;
    int64_t mem_end = 0;
    int64_t mem_count = 0;

    while (mem_end <= pattern_size - 1) {
        int64_t steps_fw =
            forward_backward(mt_reversed, mt_reversed_length, mem_start,
                             pattern, pattern_size, FORWARD);
        mem_end = mem_start + steps_fw;

        if (steps_fw >= min_mem_len) {
            mem_count++;
            // printf("MEM (start=%ld, end=%ld, len=%ld): ", mem_start, mem_end,
            // steps_fw);
            range_print_string(pattern, mem_start, mem_end);
        }

        if (mem_end >= pattern_size) {
            break;
        }

        int64_t steps_bw =
            forward_backward(mt_straight, mt_straight_length, mem_end, pattern,
                             pattern_size, BACKWARD);
        int64_t new_mem_start = mem_end - steps_bw + 1;

        mem_start = new_mem_start;
    }

    return mem_count;
}

int main(int argc, char **argv) {

    if (argc != 5) {
        fprintf(stderr,
                "\nUsage: %s <move table file> <reversed move table file> "
                "<Pattern file> "
                "<Min Mem Len>\n",
                argv[0]);
        fprintf(
            stderr,
            "\nThe MEM finder reports MEMs found within the pattern by \n"
            "reporting the starting index, ending index, and the length of \n"
            "the MEM within the pattern. The starting index is inclusive and \n"
            "ending index is exclusive: [mem_start, mem_end).\n\n");
        return EXIT_FAILURE;
    }

    int min_mem_len = atoi(argv[4]);

    props p1 = read_mvt(argv[1]);
    props p2 = read_mvt(argv[2]);

    FILE *pat_file = fopen(argv[3], "r");
    char *pat = NULL;
    size_t len_allocated = 0;
    ssize_t pat_length;
    uint8_t pat_count = 0;

    while ((pat_length = getline(&pat, &len_allocated, pat_file)) != -1) {

        pat_count++;
        printf("MEMs for Pattern %d\n", pat_count);
        printf("============================================\n");
        find_mems(p1.table, p1.r, p2.table, p2.r, pat, pat_length, min_mem_len);
        printf("\n");
    }

    if (pat)
        free(pat);

    return EXIT_SUCCESS;
}
