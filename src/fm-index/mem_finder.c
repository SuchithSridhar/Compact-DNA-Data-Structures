/**
 * MEM finder built using FM Indexes.
 * @author Suchith Sridhar (B00932400)
 * @author Anas Alhadi (B00895875)
 * @date 22 Sep 2024
 */
#define _GNU_SOURCE
#include "fm_index.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

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
  int diff = e - s;

  int64_t sn = s;
  int64_t en = e;

  while (s <= e && pi < pattern_size && pi >= 0) {
    int64_t si = fm->symbol_indices[pattern[pi]];

    int64_t rank_s = fm_index_rank(fm, pattern[pi], s - 1);
    int64_t rank_e = fm_index_rank(fm, pattern[pi], e);
    sn = scc[si] + rank_s + 1;
    en = scc[si] + rank_e;

    // printf("\t[%c] s: %ld -> %ld, e: %ld -> %ld e-s=%ld\n", pattern[pi], s,
    // sn,
    //       e, en, en - sn);

    s = sn;
    e = en;

    if (s > e)
      break;

    pi += direction;
    counter++;
  }

  printf("forward_backward(start= %ld, direction: %d) -> %d\n", start,
         direction, counter);
  return counter;
}

/**
 * Find maximal exact matches (MEMs) for a given pattern within a text.
 */
int find_mems(fm_index *fm_index_straight, fm_index *fm_index_reversed,
              char *pattern, size_t pattern_size, int min_mem_length) {
  int64_t mem_start = 0;
  int64_t mem_end = 0;
  int mem_count = 0;

  while (mem_end <= pattern_size - 1) {
    int64_t steps_fw = forward_backward(fm_index_reversed, mem_start, pattern,
                                        pattern_size, FORWARD);
    mem_end = mem_start + steps_fw;

    if (steps_fw >= min_mem_length) {
      mem_count++;
      printf("MEM (start=%ld, end=%ld, len=%ld): ", mem_start, mem_end,
             steps_fw);
      for (int i = mem_start; i < mem_end; ++i) {
        putchar(pattern[i]);
      }
      printf("\n");
    }

    if (mem_end >= pattern_size) {
      break;
    }

    int64_t steps_bw = forward_backward(fm_index_straight, mem_end, pattern,
                                        pattern_size, BACKWARD);
    int64_t new_mem_start = mem_end - steps_bw + 1;

    mem_start = new_mem_start;
  }
  return mem_count;
}

int main(int argc, char **argv) {
  if (argc != 5) {
    fprintf(stderr,
            "\nUsage: %s <BWT file> <Reversed BWT file> <Pattern file> <Min "
            "Mem Len>\n",
            argv[0]);
    fprintf(stderr,
            "\nThe MEM finder reports MEMs found within the pattern by\n"
            "reporting the starting index, ending index, and the length of\n"
            "the MEM within the pattern. The starting index is inclusive and\n"
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
    fprintf(stderr, "Failed to initialize reverse FM-index from %s\n", argv[2]);
    fm_index_destroy(fm);
    return EXIT_FAILURE;
  }

  int min_mem_len = atoi(argv[4]);

  FILE *pattern_file = fopen(argv[3], "r");

  char *pattern = NULL;
  size_t len_allocated = 0;
  ssize_t characters_read;
  uint8_t pattern_count = 0;

  double total_time = 0.0;

  while ((characters_read = getline(&pattern, &len_allocated, pattern_file)) !=
         -1) {
    pattern_count++;
    // Get the actual length of the line content
    printf("MEMs for Pattern %d\n", pattern_count);
    printf("============================================\n");
    clock_t start_time = clock();
    find_mems(fm, rev_fm, pattern, characters_read, min_mem_len);
    clock_t end_time = clock();
    total_time += ((double)(end_time - start_time)) / CLOCKS_PER_SEC * 1000.0;
    printf("\n");
  }

  printf("Time taken to find all MEMs: %.2f ms\n", total_time);

  if (pattern)
    free(pattern);

  fm_index_destroy(fm);
  fm_index_destroy(rev_fm);
  fclose(pattern_file);

  return EXIT_SUCCESS;
}
