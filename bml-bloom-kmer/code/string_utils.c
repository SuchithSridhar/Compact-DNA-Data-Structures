#include "string_utils.h"
#include <stdlib.h>
#include <string.h>

void range_print_string(char *string, int start, int end) {
    char tmp = string[end];
    string[end] = '\0';
    fprintf(stdout, "%s\n", &string[start]);
    string[end] = tmp;
}

char *get_substring(char *string, size_t start, size_t end) {
    char *substring = malloc(end - start + 1);
    memcpy(substring, &string[start], end);
    substring[end] = '\0';
    return substring;
}
