#ifndef RLFMINDEX_H
#define RLFMINDEX_H

#include <sdsl/bit_vectors.hpp>
#include <sdsl/int_vector.hpp>
#include <sdsl/rank_support.hpp>
#include <sdsl/sd_vector.hpp>
#include <utility>

class RLFMIndex {
public:
  int n = 0;
  int r = 0;
  int sigma = 0;
  bool loaded_B_FL = false;

  typedef struct {
    long i;
    char X;
  } LF_result_t;

  typedef struct {
    long s;
    long e;
  } range_result_t;

  typedef struct {
    int run;
    int offset;
    char X;
  } LFM_result_t;

  RLFMIndex();
  LF_result_t LF(long i);
  long LFC(long i, char x);
  void load_index(char *input_filename, bool load_B_FL);
  long select_bf(int index);
  long select_bl(int index);
  long rank_bf(int index);
  long rank_bl(int index);
  range_result_t rangeUpdate(long s, long e, char x);
  LFM_result_t LFM(int run, int offset);

private:
  // all the characters in T or BWT(T)
  std::vector<unsigned char> alphabet;

  // to map a character to the index
  // Ex. #:0, $:1, A:2, C:3, G:4, T:5
  std::vector<int> char_to_index;

  // The data structures needed for the assignment
  sdsl::sd_vector<> B_F_sparse;
  sdsl::sd_vector<> B_L_sparse;

  std::vector<int> C;
  std::vector<char> H_L;
  std::vector<std::unique_ptr<sdsl::bit_vector>> B_x;

  // Rank and Select data structures on B_F and B_L
  sdsl::rank_support_sd<> rank_B_L;
  sdsl::select_support_sd<> select_B_L;
  sdsl::rank_support_sd<> rank_B_F;
  sdsl::select_support_sd<> select_B_F;

  // Rank data structure for B_x bit vectors
  std::vector<std::unique_ptr<sdsl::rank_support_v<>>> B_x_ranks;
  sdsl::bit_vector B_FL;
  sdsl::select_support_mcl<> select_B_FL;
};

#endif
