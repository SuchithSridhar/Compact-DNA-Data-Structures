#ifndef BITVECTOR_H
#define BITVECTOR_H

#include <stdint.h>
#include <stdlib.h>

typedef struct {
    u_int8_t *bit_array;
    size_t bits;
    size_t bytes_allocated;
} bitvector_t;

bitvector_t *bv_create(size_t bits);
void bv_destroy(bitvector_t *bv);
void bv_clear_bit(bitvector_t *bv, size_t index);
void bv_set_bit(bitvector_t *bv, size_t index);
u_int8_t bv_get_bit(bitvector_t *bv, size_t index);
void bv_print(bitvector_t *bv);

#endif
