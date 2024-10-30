#ifndef __MOVE_CTOR_H
#define __MOVE_CTOR_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  uint32_t ptr;
  char head;
  uint8_t length;
  uint8_t offset;
} move_table_row_t;

typedef struct {
  move_table_row_t *table;
  size_t length;
} move_table_t;

move_table_t *move_table_create(char *bwt, size_t len);
void move_table_print(move_table_t *mt);

void move_table_store(move_table_t *mt, char *filename, size_t bwt_len);
move_table_t *move_table_load(char *filename);

void move_table_destroy(move_table_t *move_table);

#endif
