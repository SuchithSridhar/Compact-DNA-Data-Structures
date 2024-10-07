#include "file_utils.h"
#include "move_table.h"
#include <stdio.h>
#include <stdlib.h>

int move_table_count_occurences(move_table_t *move_table, char *pattern,
                                size_t len, size_t bwt_len) {

    uint32_t sr = 0;
    uint32_t er = move_table->length - 1;
    size_t si = 0;
    size_t ei = move_table->table[er].length - 1;

    printf("%zu\n", move_table->length);
    printf("%u\n", move_table->table[er].length);

    printf("inital: sr=%u si=%zu er=%u ei=%zu\n", sr, si, er, ei);

    for (size_t i = 0; i < len; i++) {
        char c = pattern[len - i - 1];

        while (move_table->table[sr].head != c) {
            sr += 1;
            si = 0;
        }

        while (move_table->table[er].head != c) {
            er -= 1;
            ei = move_table->table[er].length - 1;
        }

        uint8_t s_off = move_table->table[sr].offset;
        uint8_t e_off = move_table->table[sr].offset;
        sr = move_table->table[sr].ptr;
        er = move_table->table[er].ptr;
        ei += e_off;
        si += s_off;

        while (si > move_table->table[sr].length) {
            si -= move_table->table[sr].length;
            sr += 1;
        }

        while (ei > move_table->table[er].length) {
            ei -= move_table->table[er].length;
            er += 1;
        }

        printf("%zu: sr=%u si=%zu er=%u ei=%zu ei-si+1=%zu\n", i, sr, si, er,
               ei, ei - si + 1);
    }

    printf("returned: %zu\n", ei - si + 1);
    return ei - si + 1;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "\nUsage: %s <bwt data file>\n", argv[0]);
        fprintf(
            stderr,
            "Assuming strings end with $ and entire sequence ending with #.");
        return EXIT_FAILURE;
    }

    char *bwt_filename = argv[1];
    Text bwt = file_utils_read(bwt_filename);

    if (bwt.T == NULL) {
        fprintf(stderr, "Unable to read file: %s\n", bwt_filename);
        return EXIT_FAILURE;
    }

    move_table_t *move_table = move_table_create(bwt.T, bwt.len);

    move_table_print(move_table);

    move_table_count_occurences(move_table, "AT", 3, bwt.len);

    move_table_destroy(move_table);
    free(bwt.T);
}
