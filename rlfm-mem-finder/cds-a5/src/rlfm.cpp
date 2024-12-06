#include "rlfm.h"
#include <cstdio>
#include <sdsl/bit_vectors.hpp>
#include <sdsl/int_vector.hpp>
#include <sdsl/rank_support.hpp>
#include <sdsl/sd_vector.hpp>
#include <utility>

#define CHAR_COUNT 256

RLFMIndex::RLFMIndex() = default;

RLFMIndex::lf_result_t RLFMIndex::LF(long i) {
  int run = rank_B_L(i + 1) - 1;
  int offset = i - select_B_L(run + 1);

  char mapped_char = H_L[run];
  char X = char_to_index[mapped_char];

  long pred_x = (*B_x_ranks[X])(run);

  int run_f = C[X] + pred_x;
  long pos_f = select_B_F(run_f + 1) + offset;

  return {.i = pos_f, .x = mapped_char};
}

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

std::pair<long, long> RLFMIndex::rangeUpdate(long s, long e, char x) {
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
    return std::make_pair(-1, -1);
  }

  char xi = char_to_index[x];

  long pred_s = (*B_x_ranks[xi])(run_s);
  long pred_e = (*B_x_ranks[xi])(run_e);

  int run_sf = C[xi] + pred_s;
  long pos_sf = select_bf(run_sf + 1) + off_s;

  int run_ef = C[xi] + pred_e;
  long pos_ef = select_bf(run_ef + 1) + off_e;

  return std::make_pair(pos_sf, pos_ef);
}

long RLFMIndex::LFC(long i, char x) {
  if (i == -1)
    i = 0;
  int run = rank_B_L(i + 1) - 1;
  int offset = i - (run + 1 <= r ? select_B_L(run + 1) : n - 1);

  char X = char_to_index[x];

  long pred_x = (*B_x_ranks[X])(run);

  int run_f = C[X] + pred_x;
  long pos_f = run_f + 1 <= r ? select_B_F(run_f + 1) : n - 1;

  printf("\t\trun=%d offset=%d char=%d pred_x=%ld C[X]=%d run_f=%d pos_f=%ld\n",
         run, offset, X, pred_x, C[X], run_f, pos_f);

  return pos_f;
}

void RLFMIndex::load_index(char *input_filename) {
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

  in_file.close();
}
