#include "fm_index.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEM_LENGTH 1
#define TEXT_LENGTH 1000000

#define min(a, b)                                                              \
  ({                                                                           \
    __typeof__(a) _a = (a);                                                    \
    __typeof__(b) _b = (b);                                                    \
    _a < _b ? _a : _b;                                                         \
  })

uint64_t MEMs(char *file, char *rev_file, char *pattern) {
  fm_index *fm = build(file);
  fm_index *rev_fm = build(rev_file);
  uint64_t mem_counts = 0;
  uint32_t current_index = 0;

  while (current_index < strlen(pattern)) {

    // we use i to keep track of where the end of a MEM is
    uint32_t i = current_index;

    // check to see if the substring of length 40 occurs in text. If it doesn't,
    // then that subtring cannot be a MEM of length at least 40, so move to the
    // next MEM.
    int s = 0;
    int e = TEXT_LENGTH - 1;
    while (i < strlen(pattern) && s <= e) {
      s = rev_fm->C_array[char_to_nucleotide(pattern[i])] +
          rank(rev_fm, char_to_nucleotide(pattern[i]), s - 1) + 1;
      e = rev_fm->C_array[char_to_nucleotide(pattern[i])] +
          rank(rev_fm, char_to_nucleotide(pattern[i]), e);

      // if s > e here, then i = end + 1
      if (s > e)
        break;

      i++;
    }

    // check to see if the mem is at least 40 characters long
    if (i - current_index >= MEM_LENGTH) {
      printf("(%d, %d)\n", current_index, i - 1);
      mem_counts++;
    }

    // if we reach the EOS, then the entire pattern is a MEM
    if (i == strlen(pattern)) {
      break;
    }

    s = 0;
    e = TEXT_LENGTH - 1;
    uint32_t j = i;
    while (j >= 0 && s <= e) {
      s = fm->C_array[char_to_nucleotide(pattern[j])] +
          rank(fm, char_to_nucleotide(pattern[j]), s - 1) + 1;
      e = fm->C_array[char_to_nucleotide(pattern[j])] +
          rank(fm, char_to_nucleotide(pattern[j]), e);

      // if s > e here, then j + 1 is the start of a new MEM
      if (s > e)
        break;

      j--;
    }

    if (j == i) {
      current_index = j;
    } else {
      current_index = j + 1;
    }
  }

  free_fm_index(fm);
  free_fm_index(rev_fm);
  return mem_counts;
}

int main(int argc, char **argv) {
  FILE *pattern_file = fopen(argv[3], "r");
  size_t max_pattern_length = 10000;
  char *pattern = malloc(max_pattern_length);

  getline(&pattern, &max_pattern_length, pattern_file);
  printf("mem counts: %lld", MEMs(argv[1], argv[2], pattern));

  free(pattern);
  fclose(pattern_file);
}
