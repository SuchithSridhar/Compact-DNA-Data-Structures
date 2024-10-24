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

hash_function_t hash_functions[] = {
    {59515879, 95578445, 56887169},
    {65835776, 22503992, 34543247},
    {63372674, 9287645, 31428521},
    {58184635, 49547574, 64362491},
    {79157700, 94338060, 15976133},
    {88994189, 67949655, 63991913}
};

range_t *findValidSubstrings();
int windowLeft(char leftChar, char rightChar, int w);
int checkFilter(int w);


int PATTERN_LENGTH;
char *PATTERN;
int MIN_MEM_LENGTH;
int k;
int pow5;
int insPar;
int window;

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
      window = windowLeft(PATTERN[i], (i + k <= x + MIN_MEM_LENGTH - 1 ? PATTERN[i + k] : '#'), window);

      if (i <= x + MIN_MEM_LENGTH - k &&
          ((94607073 * (long long)window + 54204618) % 55586519) % insPar ==
              0 && !checkFilter(window)){
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

  printf("Usage: ./memFinder <pattern file> <k> <insPar> <min_mem_length> <text file>\n");

  // load parameters
  char* patternFileName = argv[1];
  k = atoi(argv[2]);
  insPar = atoi(argv[3]);
  MIN_MEM_LENGTH = atoi(argv[4]);
  char* textFileName = argv[5];
  pow5 = 1;
  for (int i = 1; i < k; i++) {
    pow5 *= 5;
  }


  // load text file 
  FILE *textFile = fopen(textFileName, "r");
  fseek(textFile, 0, SEEK_END);
  int n = (int)ftell(textFile);
  rewind(textFile);
  char* T = (char *)malloc(n);
  fread(T, 1, n, textFile);
  fclose(textFile);

  // create bloom filter ?
  bloom_filter = bf_create(8, hash_functions, 6);
  int createWindow = 0;
  int tupleCount = 0;
  for (int i = n - 1; i >= 0; i--) {
    createWindow = windowLeft(T[i], (i + k < n ? T[i + k] : '#'), createWindow);
    if (((94607073 * (long long)createWindow + 54204618) % 55586519) % insPar == 0 && !checkFilter(createWindow)) {
        long long v = (long long)createWindow;
        bf_insert(bloom_filter, v);
        tupleCount++;
    }
  }
  printf("Filter built, %i %i-tuples added to filter.\n", tupleCount, k);

  // reading each pattern and finding valid substrings
  FILE *patternFile = fopen(patternFileName, "r");
  if (patternFile == NULL) {
      perror("Unable to open pattern file");
      return EXIT_FAILURE;
  }
  char *line = NULL;
  size_t len = 0;
  size_t read;
  while ((read = getline(&line, &len, patternFile)) != -1) {    // Remove newline character if present
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

  free(line);
  free(PATTERN);
  free(T);

  bf_destroy(bloom_filter);
  return 0;
}

int windowLeft(char leftChar, char rightChar, int w) {

  switch (rightChar) {
  case 'A':
    w -= 1;
    break;
  case 'C':
    w -= 2;
    break;
  case 'G':
    w -= 3;
    break;
  case 'T':
    w -= 4;
    break;
  }

  w = w / 5;

  switch (leftChar) {
  case 'A':
    w += 1 * pow5;
    break;
  case 'C':
    w += 2 * pow5;
    break;
  case 'G':
    w += 3 * pow5;
    break;
  case 'T':
    w += 4 * pow5;
    break;
  }

  return w;
}

int checkFilter(int w) {
    long long v = (long long)w;
    return (int)bf_query(bloom_filter, v);
}
