// TODO: This shouldn't be it's own file since if we make changes to the
//  way we find mems then this will have to be separately updated.
#include "kmer_filter.h"
#include "string_utils.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define FORWARD 1
#define BACKWARD -1
#define MILLISECOND_SCALE 1000

// shouldnt use global but this is just a make shift solution to avoid having to
// change func signatures for now, will only be used to cound backward steps in
// find_mem()
int backward_steps_call = 0;
int backward_steps = 0;

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

/**
 * @brief Creates a move table from a binary move table file
 *
 * @param[in] filename name of the binary move table file
 * @return A move table
 *
 */
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
 * @brief Find maximal exact matches (MEMs) for a given pattern within a text.
 *
 * @param[in] mvt_straight move table of the original text
 * @param[in] mvt_straight_length length of the move table of the original text
 * @param[in] mvt_reversed move table of the reversed text
 * @param[in] mvt_reversed_length length of the move table of the reversed text
 * @param[in] pattern
 * @param[in] pattern_size length of the pattern
 * @return the number of MEMs found for the given pattern
 *
 */
int find_mems(move_table_t *mvt_straight, int mvt_straight_length,
              move_table_t *mvt_reversed, int mvt_reversed_length,
              char *pattern, size_t pattern_size, int min_mem_len) {
  int64_t mem_start = 0;
  int64_t mem_end = 0;
  int64_t mem_count = 0;

  // Loop variables
  int64_t steps_fw;
  int64_t steps_bw;
  int64_t mem_boundary;

  while (mem_start + min_mem_len - 1 <= pattern_size) {
    mem_boundary = mem_start + min_mem_len - 1;
    steps_bw = forward_backward(mvt_straight, mvt_straight_length, mem_boundary,
                                pattern, pattern_size, BACKWARD);

    if (steps_bw < min_mem_len) {
      mem_start = mem_boundary - steps_bw + 1;
      continue;
    }

    // gives the end of the current valid mem
    steps_fw = forward_backward(mvt_reversed, mvt_reversed_length, mem_start,
                                pattern, pattern_size, FORWARD);
    mem_end = mem_start + steps_fw;

    range_print_string(pattern, mem_start, mem_end);
    mem_count++;

    // setting the start of the next mem
    if (mem_start + steps_fw >= pattern_size - 1) {
      break;
    }
    steps_bw = forward_backward(mvt_straight, mvt_straight_length, mem_end,
                                pattern, pattern_size, BACKWARD);
    mem_start = mem_end - steps_bw + 1;
  }

  // code bellow uses forward backward
  /*
          while (mem_end <= pattern_size - 1) {
                  int64_t steps_fw =
              forward_backward(mvt_reversed, mvt_reversed_length, mem_start,
                               pattern, pattern_size, FORWARD);
          mem_end = mem_start + steps_fw;

          if (steps_fw >= min_mem_len) {
              mem_count++;
              range_print_string(pattern, mem_start, mem_end);
          }

          if (mem_end >= pattern_size) {
              break;
          }

          int64_t steps_bw =
              forward_backward(mvt_straight, mvt_straight_length, mem_end,
                               pattern, pattern_size, BACKWARD);
          backward_steps += steps_bw;
          backward_steps_call++;

          int64_t new_mem_start = mem_end - steps_bw + 1;

          mem_start = new_mem_start;
      }

  */
  printf("mems found = %ld\n", mem_count);
  return mem_count;
}

/**
 * @brief Finds the Maximal Exact Matches (MEMs) of a text given a pattern using
 * a kmer bloom filter.
 *
 * Finds maximal substrings of the pattern that occurs in the text and finds the
 * MEMs in each of the substrings.
 *
 * @param[in] mt_straight move table of the original text
 * @param[in] mt_straight_length length of the move table of the original text
 * @param[in] mt_reversed move table of the reversed text
 * @param[in] mt_reversed_length length of the move table of the reversed text
 * @param[in] kmer_filter
 * @param[in] pattern the pattern used to match again the text
 * @param[in] min_mem_length minimum length of the MEMs to be found
 *
 */
void find_substrings(move_table_t *mt_straight, int mt_straight_length,
                     move_table_t *mt_reversed, int mt_reversed_length,
                     kmer_filter_t *kmer_filter, char *pattern, int pat_len,
                     int min_mem_len) {
  kmer_filter_t *kf = kmer_filter;

  size_t start_substring = 0;
  size_t end_substring = 0;

  for (size_t i = 0; i < pat_len - kf->kmer_size; i++) {
    kmer_int_t kmer = kmerf_as_int(kf, pattern, i);
    if (kmerf_should_contain(kf, kmer) && !kmerf_query(kf, kmer)) {
      // a kmer not found in bloom filter
      end_substring = i + kf->kmer_size;
      if (end_substring - start_substring >= min_mem_len) {
        find_mems(mt_straight, mt_straight_length, mt_reversed,
                  mt_reversed_length, pattern + start_substring,
                  end_substring - start_substring, min_mem_len);
      }

      start_substring = i + 1;
    }
  }

  if (pat_len - start_substring >= min_mem_len) {
    find_mems(mt_straight, mt_straight_length, mt_reversed, mt_reversed_length,
              pattern + start_substring, pat_len - start_substring,
              min_mem_len);
  }
}

int main(int argc, char **argv) {

  if (argc != 6) {
    fprintf(stderr,
            "\nUsage: %s <move table file> <reversed move table file> "
            "\n<kmer filter file> "
            "<Pattern file> "
            "<Min Mem Len>\n",
            argv[0]);
    fprintf(stderr,
            "\nThe MEM finder reports MEMs found within the pattern by \n"
            "reporting the starting index, ending index, and the length of \n"
            "the MEM within the pattern. The starting index is inclusive and \n"
            "ending index is exclusive: [mem_start, mem_end).\n\n");
    return EXIT_FAILURE;
  }

  int min_mem_len = atoi(argv[5]);

  props p1 = read_mvt(argv[1]);
  props p2 = read_mvt(argv[2]);

  FILE *pat_file = fopen(argv[4], "r");
  char *pat = NULL;
  size_t len_allocated = 0;
  ssize_t pat_length;
  uint8_t pat_count = 0;

  char *kmer_filter_file = argv[3];
  kmer_filter_t kmer_filter;
  kmerf_load_file(&kmer_filter, kmer_filter_file);

  double total_clock_time = 0;

  while ((pat_length = getline(&pat, &len_allocated, pat_file)) != -1) {
    pat_count++;
    printf("MEMs for Pattern %d\n", pat_count);
    printf("============================================\n");

    clock_t start = clock();
    find_substrings(p1.table, p1.r, p2.table, p2.r, &kmer_filter, pat,
                    pat_length, min_mem_len);
    clock_t end = clock();

    total_clock_time += (double)(end - start);

    printf("\n");
  }

  printf("Time taken to find all mems: %lf ms\n",
         total_clock_time / CLOCKS_PER_SEC * MILLISECOND_SCALE);
  printf("Number of backward steps: %d\n", backward_steps);
  if (pat)
    free(pat);

  return EXIT_SUCCESS;
}
