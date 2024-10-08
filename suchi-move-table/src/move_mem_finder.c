#include "file_utils.h"
#include "move_table.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_PATTERN_LENGTH 1000000
#define MIN_MEM_LENGTH 40

#define FORWARD 1
#define BACKWARD -1

#define SLTEE(s_run, s_offset, e_run, e_offset)                                \
    ((s_run) < (e_run) || ((s_run) == (e_run) && (s_offset) <= (e_offset)))

// direction = 1 -> right (forward)
// direction = -1 -> left (backward)
// I'm going to assume you gave me the right fm index
// returns the steps I was able to walk
int forward_backward(move_table_t *mt, int64_t start, char *pattern,
                     size_t pattern_size, int8_t direction) {

    int64_t s_run = 0;
    int16_t s_offset = 0;
    int64_t e_run = mt->length - 1;
    int16_t e_offset = mt->table[e_run].length - 1;

    int64_t mt_len = mt->length;
    move_table_row_t *mtb = mt->table;

    int pi = start;
    int counter = 0;

    while (SLTEE(s_run, s_offset, e_run, e_offset) && pi < pattern_size &&
           pi >= 0) {
        printf("dir: %d, s_run: %ld, s_off %d, e_run: %ld, e_off: %d\n",
               direction, s_run, s_offset, e_run, e_offset);

        // if head char != looking char, move s down table
        while (mtb[s_run].head != pattern[pi]) {
            s_run++;
            s_offset = 0;
            assert(s_run < mt_len);
        }

        // if head char != looking char, move e up table
        while (mtb[e_run].head != pattern[pi]) {
            e_run--;
            e_offset = mtb[e_run].length - 1;
            assert(e_run >= 0);
        }

        if (!SLTEE(s_run, s_offset, e_run, e_offset))
            break;

        // jump for s using move table
        s_offset += mtb[s_run].offset;
        s_run = mtb[s_run].ptr;

        // while s is off the current run, move to next run
        while (s_offset >= mtb[s_run].length) {
            s_offset -= mtb[s_run].length;
            s_run++;
        }

        // jump for e using move table
        e_offset += mtb[e_run].offset;
        e_run = mtb[e_run].ptr;

        // while e is off the current run, move to next run
        while (e_offset >= mtb[e_run].length) {
            e_offset -= mtb[e_run].length;
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
int find_mems(move_table_t *mt_straight, move_table_t *mt_reversed,
              char *pattern, size_t pattern_size) {
    int64_t mem_start = 0;
    int64_t mem_end = 0;

    printf("HI there!\n");
    while (mem_end <= pattern_size - 1) {
        int64_t steps_fw = forward_backward(mt_reversed, mem_start, pattern,
                                            pattern_size, FORWARD);
        mem_end = mem_start + steps_fw;

        if (steps_fw >= MIN_MEM_LENGTH) {
            printf("MEM (start=%ld, end=%ld, len=%ld)\n", mem_start, mem_end,
                   steps_fw);
        }

        if (mem_end >= pattern_size) {
            break;
        }

        int64_t steps_bw = forward_backward(mt_straight, mem_end, pattern,
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

    char *bwt_straight_filename = argv[1];
    char *bwt_reversed_filename = argv[2];

    Text bwt_straight = file_utils_read(bwt_straight_filename);
    Text bwt_reversed = file_utils_read(bwt_reversed_filename);

    if (bwt_straight.T == NULL || bwt_reversed.T == NULL) {
        fprintf(stderr, "Unable to read bwt files.\n");
        return EXIT_FAILURE;
    }

    printf("BUILDING STRAIGHT\n");
    move_table_t *mt_straight =
        move_table_create(bwt_straight.T, bwt_straight.len);
    printf("BUILDING REVERSED\n");
    move_table_t *mt_reversed =
        move_table_create(bwt_reversed.T, bwt_reversed.len);
    printf("DONE\n");

    char *pattern = malloc(MAX_PATTERN_LENGTH * sizeof(char));
    if (!pattern) {
        fprintf(stderr, "Failed to allocate memory for pattern\n");
        move_table_destroy(mt_straight);
        move_table_destroy(mt_reversed);
        return EXIT_FAILURE;
    }

    size_t read_len = fread(pattern, sizeof(char), MAX_PATTERN_LENGTH, stdin);
    if (read_len == 0) {
        fprintf(stderr, "No pattern data read from stdin\n");
        free(pattern);
        move_table_destroy(mt_straight);
        move_table_destroy(mt_reversed);
        return EXIT_FAILURE;
    }

    find_mems(mt_straight, mt_reversed, pattern, read_len);

    free(pattern);
    move_table_destroy(mt_straight);
    move_table_destroy(mt_reversed);

    return EXIT_SUCCESS;
}
