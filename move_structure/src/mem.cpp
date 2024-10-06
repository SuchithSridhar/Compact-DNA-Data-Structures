#include <LF_move.hpp>
#include <cstdint>
#include <cstdio>
#include <sstream>

long get_file_length(std::ifstream &file) {
  if (!file.is_open()) {
    return -1;
  }

  file.seekg(0, std::ios::end);
  long length = file.tellg();

  file.seekg(0, std::ios::beg);

  return length;
}

int main() {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
    return 1;
  }

  std::string filename = argv[1];

  std::string bwt_fname = filename + ".bwt";
  std::ifstream ifs_bwt(bwt_fname);

  ifs_bwt.seekg(0);
  cout << "Reading BWT from " << bwt_fname << std::endl;
  LF_move tbl(ifs_bwt);

  tbl.bwt_stats();

  std::string pattern_file = argv[2];
  std::ifstream pattern_stream(pattern_file);

  if (!pattern) {
    std::cerr << "Could not open the file: " << filename << std::endl;
    return 1;
  }

  std::stringstream pattern_sstream;
  pattern_sstream << pattern.read();

  std::string pattern = pattern_sstream.str();
  unit16_t s = 0, e = get_file_length(filename) - 1;
  uint32_t sr = 0, er = tbl.length();

  while (s <= e) {
    while ()
  }

  std::string tbl_outfile = filename + tbl.get_file_extension();
  std::ofstream out_fp(tbl_outfile);
  tbl.serialize(out_fp);
  out_fp.close();

  return 0;
}
