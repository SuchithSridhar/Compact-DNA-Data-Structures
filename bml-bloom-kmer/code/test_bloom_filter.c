#include "bloom_filter.h"
#include <assert.h>
#include <stdio.h>

hash_function_t hash_funcs[] = {{.A = 59515879, .B = 95578445, .P = 56887169},
                                {.A = 65835776, .B = 22503992, .P = 34543247},
                                {.A = 63372674, .B = 9287645, .P = 31428521},
                                {.A = 58184635, .B = 49547574, .P = 64362491},
                                {.A = 79157700, .B = 94338060, .P = 15976133},
                                {.A = 88994189, .B = 67949655, .P = 63991913}};

void test_bf_create_and_destroy() {
    size_t bits = 1024;
    size_t hash_count = 3;
    bloom_filter_t *bf = bf_create(bits, hash_funcs, hash_count);

    assert(bf != NULL);
    assert(bf->bv != NULL);
    assert(bf->hash_functions != NULL);
    assert(bf->hash_functions_count == hash_count);

    bf_destroy(bf);
}

void test_bf_hash() {
    hash_function_t hf = hash_funcs[0];
    int64_t value = 42;
    uint64_t hash_value = bf_hash(&hf, value);
    uint64_t hash_expected = 35322758;

    assert(hash_value < hf.P);
    assert(hash_value == hash_expected);
}

void test_bf_insert_and_query() {
    size_t bits = 1024;
    size_t hash_count = 3;
    bloom_filter_t *bf = bf_create(bits, hash_funcs, hash_count);

    int64_t value1 = 42;
    int64_t value2 = 99;

    assert(bf_query(bf, value1) == false);
    assert(bf_query(bf, value2) == false);

    bf_insert(bf, value1);

    assert(bf_query(bf, value1) == true);

    assert(bf_query(bf, value2) == false);

    bf_destroy(bf);
}

void test_multiple_insertions() {
    size_t bits = 2048;
    size_t hash_count = 5;
    bloom_filter_t *bf = bf_create(bits, hash_funcs, hash_count);

    int64_t value1 = 13;
    int64_t value2 = 89;
    int64_t value3 = 124;

    bf_insert(bf, value1);
    bf_insert(bf, value2);

    assert(bf_query(bf, value1) == true);
    assert(bf_query(bf, value2) == true);

    assert(bf_query(bf, value3) == false);

    bf_insert(bf, value3);
    assert(bf_query(bf, value3) == true);

    bf_destroy(bf);
}

int main() {
    test_bf_create_and_destroy();
    test_bf_hash();
    test_bf_insert_and_query();
    test_multiple_insertions();

    printf("All tests passed successfully.\n");
    return 0;
}
