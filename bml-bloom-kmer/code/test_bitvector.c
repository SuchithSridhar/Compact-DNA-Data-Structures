#include "bitvector.h"
#include <assert.h>
#include <stdio.h>

void test_bv_create() {
    bitvector_t *bv = bv_create(16);
    assert(bv != NULL);
    assert(bv->bits == 16);
    for (size_t i = 0; i < 16; i++) {
        assert(bv_get_bit(bv, i) == 0);
    }
    bv_destroy(bv);
    printf("test_bv_create passed!\n");
}

void test_bv_set_bit() {
    bitvector_t *bv = bv_create(16);
    bv_set_bit(bv, 0);
    bv_set_bit(bv, 15);
    assert(bv_get_bit(bv, 0) == 1);
    assert(bv_get_bit(bv, 15) == 1);
    assert(bv_get_bit(bv, 1) == 0);
    bv_destroy(bv);
    printf("test_bv_set_bit passed!\n");
}

void test_bv_clear_bit() {
    bitvector_t *bv = bv_create(16);
    bv_set_bit(bv, 0);
    bv_set_bit(bv, 15);
    bv_clear_bit(bv, 0);
    assert(bv_get_bit(bv, 0) == 0);
    assert(bv_get_bit(bv, 15) == 1);
    bv_destroy(bv);
    printf("test_bv_clear_bit passed!\n");
}

void test_bv_get_bit() {
    bitvector_t *bv = bv_create(8);
    bv_set_bit(bv, 3);
    assert(bv_get_bit(bv, 3) == 1);
    assert(bv_get_bit(bv, 2) == 0);
    bv_destroy(bv);
    printf("test_bv_get_bit passed!\n");
}

void test_bv_print() {
    bitvector_t *bv = bv_create(16);
    bv_set_bit(bv, 0);
    bv_set_bit(bv, 7);
    bv_set_bit(bv, 11);
    printf("Expected output: 10000001 00010000 \nActual output:   ");
    bv_print(bv);
    bv_destroy(bv);
    printf("test_bv_print passed!\n");
}

int main() {
    test_bv_create();
    test_bv_set_bit();
    test_bv_clear_bit();
    test_bv_get_bit();
    test_bv_print();
    printf("All tests passed!\n");
    return 0;
}
