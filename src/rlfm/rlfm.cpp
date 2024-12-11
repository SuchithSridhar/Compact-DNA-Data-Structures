#include "rlfm.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sdsl/bit_vectors.hpp>
#include <sdsl/int_vector.hpp>
#include <sdsl/rank_support.hpp>
#include <sdsl/sd_vector.hpp>
#include <utility>

#define CHAR_COUNT 256

RLFMIndex::RLFMIndex() = default;

long RLFMIndex::select_bl(int index) {
  if (index <= 0) {
    return 0;
  } else if (index >= r) {
    return n - 1;
  }
  return select_B_L(index);
}

long RLFMIndex::select_bf(int index) {
  if (index <= 0) {
    return 0;
  } else if (index >= r) {
    return n - 1;
  }
  return select_B_F(index);
}

long RLFMIndex::rank_bl(int index) {
  if (index < 0) {
    return 0;
  } else if (index >= n) {
    return r;
  }
  return rank_B_L(index);
}

long RLFMIndex::rank_bf(int index) {
  if (index < 0) {
    return 0;
  } else if (index >= n) {
    return r;
  }
  return rank_B_F(index);
}

RLFMIndex::LF_result_t RLFMIndex::LF(long i) {
  int run = rank_B_L(i + 1) - 1;
  int offset = i - select_B_L(run + 1);

  char mapped_char = H_L[run];
  char X = char_to_index[mapped_char];

  long pred_x = (*B_x_ranks[X])(run);

  int run_f = C[X] + pred_x;
  long pos_f = select_B_F(run_f + 1) + offset;

  return {pos_f, mapped_char};
}

RLFMIndex::range_result_t RLFMIndex::rangeUpdate(long s, long e, char x) {
  int run_s = rank_bl(s + 1) - 1;
  int off_s = s - select_bl(run_s + 1);

  int run_e = rank_bl(e + 1) - 1;
  int off_e = e - select_bl(run_e + 1);

  while (run_s < r && H_L[run_s] != x) {
    run_s++;
    off_s = 0;
  }

  int changed = 0;
  while (run_e >= 0 && H_L[run_e] != x) {
    run_e--;
    changed = 1;
  }

  if (changed) {
    off_e = select_bl(run_e + 2) - select_bl(run_e + 1) - 1;
  }

  if (!((run_s) < (run_e) || ((run_s) == (run_e) && (off_s) <= (off_e)))) {
    // we need to stop here, return -1 to indicate to caller
    return {-1, -1};
  }

  char xi = char_to_index[x];

  long pred_s = (*B_x_ranks[xi])(run_s);
  long pred_e = (*B_x_ranks[xi])(run_e);

  int run_sf = C[xi] + pred_s;
  long pos_sf = select_bf(run_sf + 1) + off_s;

  int run_ef = C[xi] + pred_e;
  long pos_ef = select_bf(run_ef + 1) + off_e;

  return {pos_sf, pos_ef};
}

// Unused function for now
long RLFMIndex::LFC(long i, char x) {
  if (i == -1)
    i = 0;
  int run = rank_B_L(i + 1) - 1;
  int offset = i - (run + 1 <= r ? select_B_L(run + 1) : n - 1);

  char X = char_to_index[x];

  long pred_x = (*B_x_ranks[X])(run);

  int run_f = C[X] + pred_x;
  long pos_f = run_f + 1 <= r ? select_B_F(run_f + 1) : n - 1;

  return pos_f;
}

// LF using the movelet structure
RLFMIndex::LFM_result_t RLFMIndex::LFM(int run, int offset) {
  if (!loaded_B_FL) {
    std::cerr << "B_FL not loaded\n";
    exit(1);
  }

  char mapped_char = H_L[run];
  char X = char_to_index[mapped_char];

  long pred_x = (*B_x_ranks[X])(run);

  int run_f = C[X] + pred_x;

  long pos_f = select_B_F(run_f + 1) + offset;

  int b = select_B_FL(run_f + 1); // run_f in B_FL
  int l = b - run_f - 1;          // number of 0s till run_f

  long l_pos = l + 1 <= r ? select_B_L(l + 1) : n;
  long l_next_pos = l + 2 <= r ? select_B_L(l + 2) : n;

  while (!(l_pos <= pos_f && pos_f < l_next_pos)) {
    // while pos_f doesn't lie in current run, move to next run.
    l++;
    l_pos = l_next_pos;
    l_next_pos = l + 2 <= r ? select_B_L(l + 2) : n;
  }

  return {
      .run = l,
      .offset = (int)(pos_f - l_pos),
      .X = mapped_char,
  };
}

void RLFMIndex::load_index(char *input_filename, bool load_B_FL) {
  std::ifstream in_file(input_filename, std::ios::in | std::ios::binary);

  in_file.read(reinterpret_cast<char *>(&n), sizeof(n));
  in_file.read(reinterpret_cast<char *>(&r), sizeof(r));
  in_file.read(reinterpret_cast<char *>(&sigma), sizeof(sigma));

  H_L.resize(r);
  in_file.read(reinterpret_cast<char *>(&H_L[0]), r * sizeof(H_L[0]));

  C.resize(sigma);
  in_file.read(reinterpret_cast<char *>(&C[0]), sigma * sizeof(C[0]));

  alphabet.resize(sigma);
  in_file.read(reinterpret_cast<char *>(&alphabet[0]),
               sigma * sizeof(alphabet[0]));
  char_to_index.resize(CHAR_COUNT);
  in_file.read(reinterpret_cast<char *>(&char_to_index[0]),
               CHAR_COUNT * sizeof(char_to_index[0]));

  B_L_sparse.load(in_file);

  rank_B_L = sdsl::rank_support_sd<>(&B_L_sparse);
  select_B_L = sdsl::select_support_sd<>(&B_L_sparse);

  B_F_sparse.load(in_file);

  rank_B_F = sdsl::rank_support_sd<>(&B_F_sparse);
  select_B_F = sdsl::select_support_sd<>(&B_F_sparse);

  for (int i = 0; i < sigma; i++) {
    auto new_b_vector = std::make_unique<sdsl::bit_vector>();
    new_b_vector->load(in_file);
    B_x.push_back(std::move(new_b_vector));
  }

  for (auto &B : B_x) {
    B_x_ranks.emplace_back(std::unique_ptr<sdsl::rank_support_v<>>(
        new sdsl::rank_support_v<>(B.get())));
  }

  // we only do this is necessary as sometimes we don't need this
  if (load_B_FL) {
    loaded_B_FL = true;
    B_FL.load(in_file);
    select_B_FL = sdsl::select_support_mcl<>(&B_FL);
  }

  in_file.close();
}
