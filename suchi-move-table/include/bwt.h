#ifndef __BWT_H
#define __BWT_H

#include <stdlib.h>

typedef struct {
  char *string;
  size_t length;
} bwt_t;

bwt_t *bwt_create_for(char *text, size_t len);
bwt_t *bwt_create_rev(char *text, size_t len);
void bwt_destroy(bwt_t *bwt);

#endif
