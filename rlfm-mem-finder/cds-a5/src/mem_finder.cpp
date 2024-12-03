#include "rlfm.h"
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sdsl/bit_vectors.hpp>
#include <sdsl/int_vector.hpp>
#include <sdsl/rank_support.hpp>
#include <sdsl/sd_vector.hpp>
#include <strings.h>

#define FORWARD 1
#define BACKWARD -1

int forward_backward(int64_t start, char *pattern, size_t pattern_size,
                     int8_t direction, RLFMIndex &index) {
  int64_t s = 0;
  int64_t e = index.n - 1;

  int64_t sn = s;
  int64_t en = e;

  int pi = start;
  int counter = 0;

  while (s < e && pi < pattern_size && pi >= 0) {
    char x = pattern[pi];
    sn = index.LFC(s, x);
    en = index.LFC(e, x);

    printf("\t[%c] s: %ld -> %ld, e: %ld -> %ld e-s=%ld\n", x, s, sn, e, en,
           en - sn);

    s = sn;
    e = en;

    if (s >= e)
      break;

    pi += direction;
    counter++;
  }

  printf("forward_backward(start= %ld, direction: %d) -> %d\n", start,
         direction, counter);
  return counter;
}

int find_mems(RLFMIndex &normal_index, RLFMIndex &revered_index, char *pattern,
              size_t pattern_size, int min_mem_len) {
  int64_t mem_start = 0;
  int64_t mem_end = 0;
  int64_t mem_count = 0;

  while (mem_end <= pattern_size - 1) {
    int64_t steps_fw = forward_backward(mem_start, pattern, pattern_size,
                                        FORWARD, revered_index);
    mem_end = mem_start + steps_fw;

    if (steps_fw >= min_mem_len) {
      mem_count++;
      std::cout << "MEM (start=" << mem_start << ", end=" << mem_end
                << ", len=" << steps_fw << "): ";
      for (int i = mem_start; i < mem_end; ++i) {
        std::cout << pattern[i];
      }
      std::cout << std::endl;
    }

    if (mem_end >= pattern_size) {
      break;
    }

    int64_t steps_bw = forward_backward(mem_end, pattern, pattern_size,
                                        BACKWARD, normal_index);
    int64_t new_mem_start = mem_end - steps_bw + 1;

    mem_start = new_mem_start;
    return mem_count;
  }

  return mem_count;
}

int main(int argc, char **argv) {
  if (argc != 5) {
    std::cerr << "\nUsage: " << argv[0]
              << " <rlfm index file> <reversed rlfm index file> <Pattern file> "
                 "<Min Mem Len>\n";
    std::cerr
        << "\nThe MEM finder reports MEMs found within the pattern by\n"
           "reporting the starting index, ending index, and the length of\n"
           "the MEM within the pattern. The starting index is inclusive and\n"
           "ending index is exclusive: [mem_start, mem_end).\n\n";
    return EXIT_FAILURE;
  }

  int min_mem_len = std::atoi(argv[4]);

  RLFMIndex normal_index, reversed_index;

  // Load the RLFM indexes
  normal_index.load_index(argv[1]);
  reversed_index.load_index(argv[2]);

  std::ifstream pattern_file(argv[3]);
  if (!pattern_file.is_open()) {
    std::cerr << "Error: Unable to open pattern file " << argv[3] << std::endl;
    return EXIT_FAILURE;
  }

  std::string pattern;
  int pattern_count = 0;
  double total_time = 0.0;

  while (std::getline(pattern_file, pattern)) {
    ++pattern_count;
    std::cout << "MEMs for Pattern " << pattern_count << "\n";
    std::cout << "============================================\n";

    clock_t start_time = clock();
    find_mems(normal_index, reversed_index, &pattern[0], pattern.size(),
              min_mem_len);
    clock_t end_time = clock();

    total_time +=
        static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC * 1000.0;
    std::cout << "\n";
  }

  std::cout << "Time taken to find all MEMs: " << total_time << " ms\n";

  return EXIT_SUCCESS;
}
