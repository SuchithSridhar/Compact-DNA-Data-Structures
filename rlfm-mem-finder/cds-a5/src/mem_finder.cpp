#include <sdsl/int_vector.hpp>
#include <sdsl/sd_vector.hpp>
#include <sdsl/rank_support.hpp>
#include <sdsl/bit_vectors.hpp>

#include <vector>
#include <string>

#define CHAR_COUNT 256

#define FORWARD 1
#define BACKWARD -1

int n = 0;
int r = 0;
int sigma = 0;

// all the characters in T or BWT(T)
std::vector<unsigned char> alphabet;

// to map a character to the index
// Ex. #:0, $:1, A:2, C:3, G:4, T:5
std::vector<int> char_to_index;

// The data structures needed for the assignment
sdsl::sd_vector B_F_sparse;
sdsl::sd_vector B_L_sparse;

std::vector<int> C;
std::vector<char> H_L;
std::vector<std::unique_ptr<sdsl::bit_vector> > B_x;

// Rank and Select data structures on B_F and B_L
sdsl::rank_support_sd<> rank_B_L;
sdsl::select_support_sd<> select_B_L;
sdsl::rank_support_sd<> rank_B_F;
sdsl::select_support_sd<> select_B_F;

// Rank data structure for B_x bit vectors
std::vector<std::unique_ptr<sdsl::rank_support_v< > > > B_x_ranks;


void deserialize_data(char *inputFileName) {
    std::ifstream in_file(inputFileName, std::ios::in | std::ios::binary);

    in_file.read(reinterpret_cast<char*>(&n), sizeof(n));
    in_file.read(reinterpret_cast<char*>(&r), sizeof(r));
    in_file.read(reinterpret_cast<char*>(&sigma), sizeof(sigma));
    std::cerr << "n: " << n << "\n";
    std::cerr << "r: " << r << "\n";
    std::cerr << "sigma: " << sigma << "\n";

    H_L.resize(r);
    in_file.read(reinterpret_cast<char*>(&H_L[0]), r*sizeof(H_L[0]));

    C.resize(sigma);
    in_file.read(reinterpret_cast<char*>(&C[0]), sigma*sizeof(C[0]));

    alphabet.resize(sigma);
    in_file.read(reinterpret_cast<char*>(&alphabet[0]), sigma*sizeof(alphabet[0]));
    std::cerr << "\nThe alphabet in the BWT:\n";
    for (int i = 0; i < sigma; i++)
        std::cerr << "\t" << i << " -> " << static_cast<int>(alphabet[i]) << "(" << alphabet[i] << ")\n";
    std::cerr << "\n";
    char_to_index.resize(CHAR_COUNT);
    in_file.read(reinterpret_cast<char*>(&char_to_index[0]), CHAR_COUNT*sizeof(char_to_index[0]));

    B_L_sparse.load(in_file);

    // Building the rank and select data structures for querying B_L
    rank_B_L = sdsl::rank_support_sd<>(&B_L_sparse); // usage example: rank_B_L(i) gives the rank result at index i on B_L
    select_B_L = sdsl::select_support_sd<>(&B_L_sparse); // usage example: select_B_L(i) gives the select result at index i on B_L

    B_F_sparse.load(in_file);

    // Building the rank and select data structures for querying B_L
    rank_B_F = sdsl::rank_support_sd<>(&B_F_sparse); // usage example: rank_B_L(i) gives the rank result at index i on B_L
    select_B_F = sdsl::select_support_sd<>(&B_F_sparse); // usage example: select_B_L(i) gives the select result at index i on B_L

    for (int i = 0; i < sigma; i++) {
        auto new_b_vector = std::make_unique<sdsl::bit_vector>();
        new_b_vector->load(in_file);
        B_x.push_back(std::move(new_b_vector));
    }

    // create the rank objects for the B_x bit vectors
    for (auto& B: B_x) {
        B_x_ranks.emplace_back(std::unique_ptr<sdsl::rank_support_v< > >(new sdsl::rank_support_v< >(B.get())));
    }
    // Example: code to perform rank query on B_2 at position 10:
    // std::cerr << (*B_x_ranks[2])(10) << "\n";

    in_file.close();
}

/**
typedef struct {
    long i;
    char X;
} LF_result_t;

Reminder on how to do LF mapping using RLFM

LF_result_t LF(long i) {
    int run = rank_B_L(i + 1) - 1;
    int offset = i - select_B_L(run + 1);

    char mapped_char = H_L[run];
    char X = char_to_index[mapped_char];

    long pred_x = (*B_x_ranks[X])(run);

    int run_f = C[X] + pred_x;
    long pos_f = select_B_F(run_f + 1) + offset;

    return {.i = pos_f, X = mapped_char};
}
*/

int forward_backward(int64_t start, char *pattern, size_t pattern_size, int8_t direction) {
    int64_t s = 0;
    int64_t e = n-1;

    int pi = start;
    int counter = 0;

    while (s <= e && pi < pattern_size && pi >= 0) {
        int64_t s_run = rank_B_L(s + 1) - 1;
        int64_t s_offset = s - select_B_L(s_run + 1);

        int64_t e_run = rank_B_L(e + 1) - 1;
        int64_t e_offset = e - select_B_L(e_run + 1);

        char X = char_to_index[pattern[pi]];
        int64_t pred_x_s = (*B_x_ranks[X])(s_run);
        int64_t pred_x_e = (*B_x_ranks[X])(e_run);

        s_run = C[X] + pred_x_s;
        e_run = C[X] + pred_x_e;

        int64_t s = select_B_F(run_s + 1) + s_offset;
        int64_t e = select_B_F(run_e + 1) + e_offset;

        if (s < e) break;

        pi += direction;
        counter++;
    }

    return counter;
}

// TODO: Complete function
// Pre-req: Encapsulate RLFM into simple class or struct so that
// two difference RLFM (reversed and straight) can be used interchangeably.
int find_mems(char *pattern, size_t pattern_size, int min_mem_len) {
}

// TODO: Complete function
int main() {
}
