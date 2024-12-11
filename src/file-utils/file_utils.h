#ifndef __FILE_UTILS_H
#define __FILE_UTILS_H

#include <stdio.h>
#include <stdlib.h>

typedef struct {
  char *T;
  size_t len;
} Text;

/**
 * file_utils_read(char *filename)
 * -> Text
 * read a file and its length.
 * does two passes over the file, first
 * to get length and next to get content.
 */
Text file_utils_read(char *filename);
size_t file_utils_write(char *text, char *filename);
long _file_utils_get_length(FILE *file);

#endif
