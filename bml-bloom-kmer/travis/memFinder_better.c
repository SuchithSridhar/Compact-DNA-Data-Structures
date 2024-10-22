#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "../code/include/bitvector.h"
#include "../code/include/bloom_filter.h"

#define RANGES_SIZE 10000

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

// define an array of hash_functions to be used in the bloom filter
hash_function_t hash_functions[] = {
    {59515879, 95578445, 56887169},
    {65835776, 22503992, 34543247},
    {63372674, 9287645, 31428521},
    {58184635, 49547574, 64362491},
    {79157700, 94338060, 15976133},
    {88994189, 67949655, 63991913}
};


range_t *findValidSubstrings();

// === functions for filter ===

void windowLeft(char leftChar, char rightChar);
int checkFilter();

// ============================

int PATTERN_LENGTH;
char *PATTERN;

int MIN_MEM_LENGTH;

int k;
int pow5;
int insPar;

int window;

// declare bloom filter
bloom_filter_t* bloom_filter;


void printRanges(range_t *ranges) {
  for (int i = 0; i < RANGES_SIZE; i++) {
    // assuming that init to 0
    if (ranges[i].start == 0 && ranges[i].end == 0) {
      break;
    }
    char tmp = PATTERN[ranges[i].end];
    PATTERN[ranges[i].end] = '\0';
    fprintf(stdout, "%s\n", &PATTERN[ranges[i].start]);
    PATTERN[ranges[i].end] = tmp;
  }
}

range_t *findValidSubstrings() {
  // create bloom filter
  bloom_filter = malloc(sizeof(bloom_filter_t));
  bloom_filter = bf_create(8, hash_functions, 6); 

  int x = 0;
  int xPrime = 0;
  range_t *ranges = calloc(RANGES_SIZE, sizeof(range_t));
  int range_index = 0;
  int valid_range = 1;
  
  while (x + MIN_MEM_LENGTH - 1 <= PATTERN_LENGTH - 1) {
    xPrime = x;
    window = 0;
    valid_range = 1;

    for (int i = x + MIN_MEM_LENGTH - 1; i >= x; i--) {
      windowLeft(PATTERN[i], (i + k <= x + MIN_MEM_LENGTH - 1 ? PATTERN[i + k] : '#'));

      if (i <= x + MIN_MEM_LENGTH - k &&
          ((94607073 * (long long)window + 54204618) % 55586519) % insPar ==
              0 &&
          !checkFilter()) {
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

  printf("Usage: ./memFinder <pattern file> <k> <insPar> <min_mem_length>\n");

  // load pattern and pattern length
  char* patternFileName = argv[1];
  k = atoi(argv[2]);
  insPar = atoi(argv[3]);
  MIN_MEM_LENGTH = atoi(argv[4]);


  pow5 = 1;

  for (int i = 1; i < k; i++) {
    pow5 *= 5;
  }

  FILE *patternFile = fopen("patterns.txt", "r");
  if (patternFile == NULL) {
      perror("Unable to open pattern file");
      return EXIT_FAILURE;
  }

  char *line = NULL;
  size_t len = 0;
  size_t read;

  int currPattern = 0;

  while ((read = getline(&line, &len, patternFile)) != -1) {
    currPattern++;
    printf("\n\n======= Pattern %d ========\n\n", currPattern);
    // Remove newline character if present
    if (line[read - 1] == '\n') {
        line[read - 1] = '\0';
        read--; // Adjust the length to exclude the newline character
    }

    // Allocate memory for the pattern
    PATTERN = (char *)malloc(read);
    if (PATTERN == NULL) {
        perror("Unable to allocate memory for pattern");
        exit(EXIT_FAILURE);
    }

    // Copy the line to the pattern
    strncpy(PATTERN, line, read);

    // Set the pattern length
    PATTERN_LENGTH = (int)read;

    // Call the function with the pattern and its length
    range_t* ranges = findValidSubstrings();
    printRanges(ranges);

  }

  fclose(patternFile);
  return 0;
}

void windowLeft(char leftChar, char rightChar) {

  switch (rightChar) {
  case 'A':
    window -= 1;
    break;
  case 'C':
    window -= 2;
    break;
  case 'G':
    window -= 3;
    break;
  case 'T':
    window -= 4;
    break;
  }

  window = window / 5;

  switch (leftChar) {
  case 'A':
    window += 1 * pow5;
    break;
  case 'C':
    window += 2 * pow5;
    break;
  case 'G':
    window += 3 * pow5;
    break;
  case 'T':
    window += 4 * pow5;
    break;
  }

  return;
}

int checkFilter() {
    
    long long v = (long long)window;

    return bf_query(bloom_filter, v);
}
