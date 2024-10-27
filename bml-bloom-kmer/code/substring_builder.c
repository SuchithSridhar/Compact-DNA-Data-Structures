#include "kmer_filter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct row {
    char head;
    unsigned char length;
    int pointer;
    unsigned char offset;
} row;

typedef struct searchResult {
    int matchLength;
    int sRun;
    int sOffset;
    int eRun;
    int eOffset;
} searchResult;

typedef struct range {
    int start;
    int end;
} range_t;

#define RANGES_SIZE 10000
#define MIN_MEM_LENGTH 10

void printRanges(range_t *ranges, char *pattern) {
    for (int i = 0; i < RANGES_SIZE; i++) {
        // assuming that init to 0
        if (ranges[i].start == 0 && ranges[i].end == 0) {
            break;
        }
        char tmp = pattern[ranges[i].end];
        pattern[ranges[i].end] = '\0';
        fprintf(stdout, "%s\n", &pattern[ranges[i].start]);
        pattern[ranges[i].end] = tmp;
    }
}

range_t *findValidSubstrings(kmer_filter_t *kf, char *pattern,
                             size_t pattern_length) {
    int x = 0;
    int xPrime = 0;
    range_t *ranges = calloc(RANGES_SIZE, sizeof(range_t));
    int range_index = 0;
    int valid_range = 1;

    while (x + MIN_MEM_LENGTH - 1 <= pattern_length - 1) {
        xPrime = x;
        valid_range = 1;

        for (int i = x + MIN_MEM_LENGTH - 1; i >= x; i--) {
            kmer_int_t kmer_as_int = kmerf_as_int(kf, pattern, i);

            if (i <= x + MIN_MEM_LENGTH - kf->kmer_size &&
                kmerf_should_contain(kf, kmer_as_int) &&
                !kmerf_query(kf, kmer_as_int)) {
                xPrime = i + 1;
                valid_range = 0;
                break;
            }
        }

        if (valid_range) {
            if (range_index != 0 && x < ranges[range_index - 1].end) {
                ranges[range_index - 1].end = x + MIN_MEM_LENGTH;
            } else {
                ranges[range_index].start = x;
                ranges[range_index].end = x + MIN_MEM_LENGTH;
                range_index++;
            }

            x++;

        } else {
            x = xPrime;
        }
    }

    return ranges;
}

int main(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(stderr,
                "Usage: %s <kmer_filter_file> <pattern_file> [mem length]\n",
                argv[0]);
    }

    kmer_filter_t kf;
    kmerf_load_file(&kf, argv[1]);

    FILE *patternFile = fopen(argv[2], "r");
    if (patternFile == NULL) {
        perror("Unable to open pattern file");
        return EXIT_FAILURE;
    }

    char *line = NULL;
    size_t len = 0;
    size_t read;
    while ((read = getline(&line, &len, patternFile)) != -1) {
        range_t *ranges = findValidSubstrings(&kf, line, len);
        printRanges(ranges, line);
    }

    fclose(patternFile);

    free(line);
    return 0;
}
