#include <LF_move.hpp>
#include <cstdio>

// Assumes <filename>.LF_move exists
int main(int argc, char *const argv[]) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <filename> <invert_filename"
              << std::endl;
    return 1;
  }

  std::string filename = argv[1];
  std::string invert_filename = argv[2];

  LF_move<> tbl;

  std::string move_fname = filename + tbl.get_file_extension();
  std::ifstream ifs_move(move_fname);

  ifs_move.seekg(0);
  tbl.load(ifs_move);

  tbl.bwt_stats();

  tbl.invert(invert_filename);

  return 0;
}
