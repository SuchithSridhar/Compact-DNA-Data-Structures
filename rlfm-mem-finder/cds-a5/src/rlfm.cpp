#include "rlfm.h"
#include <sdsl/bit_vectors.hpp>
#include <sdsl/int_vector.hpp>
#include <sdsl/rank_support.hpp>
#include <sdsl/sd_vector.hpp>

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

long RLFMIndex::LFC(long i, char x) {
  int run = rank_B_L(i + 1) - 1;
  int offset = i - (run + 1 <= r ? select_B_L(run + 1) : n - 1);

  char X = char_to_index[x];

  long pred_x = (*B_x_ranks[X])(run);

  int run_f = C[X] + pred_x;
  long pos_f = run_f + 1 <= r ? select_B_F(run_f + 1) : n - 1;

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
