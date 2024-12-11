#include "rlfm.h"

void invert_bwt(RLFMIndex &rlfm) {
  RLFMIndex::LF_result_t r = {
      .i = 0,
      .X = 0,
  };
  do {
    r = rlfm.LF(r.i);
    std::cout << r.X;
  } while (r.i != 0);
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "\nUsage: " << argv[0] << " <rlfm index file>\n";
    return EXIT_FAILURE;
  }

  RLFMIndex index;
  index.load_index(argv[1], false);
  invert_bwt(index);
}
