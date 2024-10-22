#include "include/bitvector.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BITS_IN_BYTE 8

int map[] = {
    0b10000000, 0b01000000, 0b00100000, 0b00010000,
    0b00001000, 0b00000100, 0b00000010, 0b00000001,
};

bitvector_t *bv_create(size_t bits) {
    bitvector_t *bv = malloc(sizeof(bitvector_t));
    if (bv == NULL) {
        return NULL;
    }

    size_t num_bytes = (bits + BITS_IN_BYTE - 1) / BITS_IN_BYTE;

    bv->bits = bits;
    bv->bytes_allocated = num_bytes;
    bv->bit_array = calloc(num_bytes, sizeof(uint8_t));

    if (bv->bit_array == NULL) {
        free(bv);
        return NULL;
    }
    return bv;
}

void bv_destroy(bitvector_t *bv) {
    if (bv == NULL) {
        return;
    }

    free(bv->bit_array);
    free(bv);
}

void bv_set_bit(bitvector_t *bv, size_t index) {
    assert(bv != NULL && index < bv->bits);
    bv->bit_array[index / BITS_IN_BYTE] |= map[index % BITS_IN_BYTE];
}

u_int8_t bv_get_bit(bitvector_t *bv, size_t index) {
    assert(bv != NULL && index < bv->bits);
    return (bv->bit_array[index / BITS_IN_BYTE] & map[index % BITS_IN_BYTE]) >
           0;
}

void bv_clear_bit(bitvector_t *bv, size_t index) {
    assert(bv != NULL && index < bv->bits);
    bv->bit_array[index / BITS_IN_BYTE] &= ~map[index % BITS_IN_BYTE];
}

void bv_print(bitvector_t *bv) {
    assert(bv != NULL && bv->bit_array != NULL);

    for (size_t i = 0; i < bv->bits; i++) {
        printf("%d", bv_get_bit(bv, i));
        if ((i + 1) % 8 == 0) {
            printf(" ");
        }
    }
    printf("\n");
}
