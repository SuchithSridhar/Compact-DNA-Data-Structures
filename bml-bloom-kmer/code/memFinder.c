#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "bitvector.h"
#include "bloom_filter.h"
#include "file_utils.h"
#include "kmer_filter.h"


#define RANGES_SIZE 10000

#define HASH_FUNCTION_COUNT 6

hash_function_t hash_funcs[HASH_FUNCTION_COUNT] = {
    {.A = 59515879, .B = 95578445, .P = 56887169},
    {.A = 65835776, .B = 22503992, .P = 34543247},
    {.A = 63372674, .B = 9287645, .P = 31428521},
    {.A = 58184635, .B = 49547574, .P = 64362491},
    {.A = 79157700, .B = 94338060, .P = 15976133},
    {.A = 88994189, .B = 67949655, .P = 63991913}};

// This hash function along with insertion parameter is used to control the
// number of kmers inserted into the filter
hash_function_t insertion_param_hash = {
    .A = 94607073, .B = 54204618, .P = 55586519};

void init_kmer_struct(kmer_filter_t *kf, size_t bv_size, int kmer_size,
                      int ins_param) {
    kf->bloom = bf_create(bv_size, hash_funcs, HASH_FUNCTION_COUNT);
    kf->kmer_size = kmer_size;
    kf->filter_size = bv_size;
    kf->_five_to_pow_km1 = 1;
    kf->_cache_source_string = NULL;
    kf->insertion_hash = insertion_param_hash;
    kf->insertion_param = ins_param;

    for (int i = 0; i < kmer_size - 1; i++) {
        kf->_five_to_pow_km1 *= 5;
    }
}

kmer_filter_t* build_kmer_filter(const char *input_file, const char *output_file, int kmer_length, size_t filter_size, int insertion_param) {
    Text t = file_utils_read(input_file);
    kmer_filter_t kmer_filter;
    init_kmer_struct(&kmer_filter, filter_size, kmer_length, insertion_param);
    kmerf_populate_result_t r = kmerf_populate(&kmer_filter, t);
    int err = kmerf_save_file(&kmer_filter, output_file);

    if (err != EXIT_SUCCESS) {
        fprintf(stderr, "Unable to write to file %s\n", output_file);
        exit(EXIT_FAILURE);
    }

    printf("Created a file %s where %zu bits set and %zu tuples inserted.\n",
           output_file, r.bits_set, r.kmers_inserted);
    
    return &kmer_filter;
}

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


range_t *findValidSubstrings();



int PATTERN_LENGTH;
char *PATTERN;
int MIN_MEM_LENGTH;
int k;
int pow5;
int insPar;
int window;

kmer_filter_t* kf;


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
    valid_range = 1;

    for (int i = x + MIN_MEM_LENGTH - 1; i >= x; i--) {
      kmer_int_t kmer_as_int = kmerf_as_int(kf, PATTERN, i);

      if (i <= x + MIN_MEM_LENGTH - k &&
          kmerf_should_contain(kf, kmer_as_int) && !kmerf_query(kf, kmer_as_int)){
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
  kf = build_kmer_filter(textFileName, "data/bloom.bml", k, 500000/8, insPar);


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

  return 0;
}
