#include "file_utils.h"
#include "move_table.h"
#include <stdio.h>
#include <stdlib.h>

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
    
    size_t bwt_len = bwt.len;
    move_table_store(move_table, "experimental.mvt", bwt_len);
    move_table_load("experimental.mvt");

    move_table_print(move_table);

    move_table_destroy(move_table);
    free(bwt.T);
}
