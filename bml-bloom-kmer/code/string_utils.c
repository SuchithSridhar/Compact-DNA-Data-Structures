#include "string_utils.h"

void range_print_string(char *string, int start, int end) {
    char tmp = string[end];
    string[end] = '\0';
    fprintf(stdout, "%s\n", &string[start]);
    string[end] = tmp;
}


