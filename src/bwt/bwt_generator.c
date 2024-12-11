#include "bwt.h"
#include "file_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum operation_t { forward, reverse, both };

int create_and_write_bwt(bwt_t *(*create_bwt_func)(char *, size_t), char *text,
                         size_t len, char *base_filename, char *suffix) {
    bwt_t *bwt = create_bwt_func(text, len);
    if (!bwt) {
        fprintf(stderr,
                "Failed to create BWT for transformation with suffix %s.\n",
                suffix);
        return EXIT_FAILURE;
    }

    size_t base_len = strlen(base_filename);
    size_t suffix_len = strlen(suffix);
    char *new_filename = malloc(sizeof(char) * (base_len + suffix_len + 1));
    if (!new_filename) {
        perror("Failed to allocate memory");
        bwt_destroy(bwt);
        return EXIT_FAILURE;
    }

    sprintf(new_filename, "%s%s", base_filename, suffix);

    if (file_utils_write(bwt->string, new_filename) == 0) {
        fprintf(stderr, "Failed to write BWT to file: %s\n", new_filename);
        free(new_filename);
        bwt_destroy(bwt);
        return EXIT_FAILURE;
    }

    free(new_filename);
    bwt_destroy(bwt);
    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "\nUsage: %s [for|rev|both] <data file>\n", argv[0]);
        fprintf(stderr, "Assuming last character of file is '$'.\n");
        fprintf(
            stderr,
            "Possible created files: <data file>.bwt <data file>.rev.bwt.\n");
        return EXIT_FAILURE;
    }

    char *operation = argv[1];
    char *filename = argv[2];
    enum operation_t op;

    if (strcmp(operation, "for") == 0) {
        op = forward;
    } else if (strcmp(operation, "rev") == 0) {
        op = reverse;
    } else if (strcmp(operation, "both") == 0) {
        op = both;
    } else {
        fprintf(stderr, "Invalid operation, must be one of: for, rev, both.\n");
        return EXIT_FAILURE;
    }

    Text text = file_utils_read(filename);
    if (text.T == NULL) {
        fprintf(stderr, "Unable to read file: %s\n", filename);
        return EXIT_FAILURE;
    }

    if (op == forward || op == both) {
        if (create_and_write_bwt(bwt_create_for, text.T, text.len, filename,
                                 ".bwt") != EXIT_SUCCESS) {
            free(text.T);
            return EXIT_FAILURE;
        }
    }

    if (op == reverse || op == both) {
        if (create_and_write_bwt(bwt_create_rev, text.T, text.len, filename,
                                 ".rev.bwt") != EXIT_SUCCESS) {
            free(text.T);
            return EXIT_FAILURE;
        }
    }

    free(text.T);
    return EXIT_SUCCESS;
}
