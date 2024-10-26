#include "file_utils.h"
#include <stdio.h>
#include <stdlib.h>

long _file_utils_get_length(FILE *file) {
    if (!file)
        return -1;

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    return length;
}

Text file_utils_read(char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        return (Text){.T = NULL, .len = -1};
    }

    Text text;
    long length = _file_utils_get_length(f);
    if (length < 0) {
        fclose(f);
        return (Text){.T = NULL, .len = -1};
    }

    text.len = length;
    text.T = malloc(sizeof(char) * (length + 1));
    if (!text.T) {
        fclose(f);
        return (Text){.T = NULL, .len = -1};
    }

    size_t read_len = fread(text.T, 1, length, f);
    text.T[read_len] = '\0';

    fclose(f);
    return text;
}

size_t file_utils_write(char *text, char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        return 0;
    }

    size_t result = (size_t)fputs(text, f);
    fclose(f);
    return result;
}
