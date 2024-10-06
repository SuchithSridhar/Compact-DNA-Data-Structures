#define _GNU_SOURCE
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *str;
    int64_t len;
} bwt_t;

long get_file_length(FILE *file) {
    if (!file) return -1;

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    return length;
}

int suffixCmp(const void *a, const void *b, void *context) {
    char *T = (char *) context;
    return(strcmp(&T[*(int *) a], &T[*(int *) b]));
}

bwt_t create_bwt(char *filename) {
    FILE *file = fopen(filename, "r");

    if (!file) return (bwt_t){.str=NULL, .len=-1};

    int64_t n = get_file_length(file);

    char *data = malloc(n + 1);
    if (!data) {
        fclose(file);
        return (bwt_t){.str=NULL, .len=-1};
    }

    fread(data, 1, n, file);
    data[n] = '\0';
    fclose(file);

    char *filtered_data = malloc(n + 1);
    if (!filtered_data) {
        free(data);
        return (bwt_t){.str=NULL, .len=-1};
    }

    int64_t filtered_len = 0;
    for (int64_t i = 0; i < n; i++) {
        if (data[i] != '\n') {
            filtered_data[filtered_len++] = data[i];
        }
    }
    filtered_data[filtered_len] = '\0';

    bwt_t bwt;
    bwt.str = malloc(filtered_len + 1);
    if (!bwt.str) {
        free(data);
        free(filtered_data);
        return (bwt_t){.str=NULL, .len=-1};
    }

    int64_t *SA = malloc(filtered_len * sizeof(int64_t));
    if (!SA) {
        free(data);
        free(filtered_data);
        free(bwt.str);
        return (bwt_t){.str=NULL, .len=-1};
    }

    for (int64_t i = 0; i < filtered_len; i++) {
        SA[i] = i;
    }

    qsort_r(SA, filtered_len, sizeof(int64_t), suffixCmp, filtered_data);

    for (int64_t i = 0; i < filtered_len; i++) {
        bwt.str[i] = filtered_data[(SA[i] + (filtered_len - 1)) % filtered_len];
    }
    bwt.str[filtered_len] = '\0';
    bwt.len = filtered_len;

    free(data);
    free(filtered_data);
    free(SA);

    return bwt;
}

int main(int argc, char *argv[]) {
    // NOTE: Program filters out any new line characters
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }

    bwt_t bwt = create_bwt(argv[1]);
    if (bwt.str == NULL || bwt.len == -1) {
        fprintf(stderr, "Error: Could not generate BWT for file '%s'\n", argv[1]);
        return EXIT_FAILURE;
    }

    printf("%s\n", bwt.str);

    free(bwt.str);

    return EXIT_SUCCESS;
}
