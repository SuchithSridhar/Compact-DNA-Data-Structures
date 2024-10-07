#include "bwt.h"
#include "sort_r.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int _bwt_suffix_cmp(const void *a, const void *b, void *T) {
    return strcmp(&((char *)T)[*(int *)a], &((char *)T)[*(int *)b]);
}

char *_bwt_rev_str(char *str, int length) {
    if (str == NULL || length < 0)
        return NULL;

    char *rev = malloc(sizeof(char) * (length + 1));
    if (rev == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < length; i++) {
        rev[i] = str[length - i - 1];
    }
    rev[length] = '\0';

    return rev;
}

bwt_t *bwt_create_for(char *text, size_t len) {
    if (text == NULL || len == 0)
        return NULL;

    bwt_t *bwt = malloc(sizeof(bwt_t));
    if (!bwt)
        return NULL;

    bwt->length = len;
    bwt->string = malloc(sizeof(char) * (len + 1));
    if (bwt->string == NULL) {
        free(bwt);
        return NULL;
    }

    int *SA = malloc(sizeof(int) * len);
    if (!SA) {
        free(bwt->string);
        free(bwt);
        return NULL;
    }

    for (size_t i = 0; i < len; i++) {
        SA[i] = i;
    }

    sort_r(SA, len, sizeof(int), _bwt_suffix_cmp, text);

    for (size_t i = 0; i < len; i++) {
        bwt->string[i] = text[(SA[i] + (len - 1)) % len];
    }
    bwt->string[len] = '\0';

    free(SA);
    return bwt;
}

bwt_t *bwt_create_rev(char *text, size_t len) {
    if (text == NULL || len == 0)
        return NULL;

    char *rev = _bwt_rev_str(text, len);
    if (!rev)
        return NULL;

    bwt_t *bwt_rev = bwt_create_for(rev, len);
    free(rev);

    return bwt_rev;
}

void bwt_destroy(bwt_t *bwt) {
    if (bwt) {
        free(bwt->string);
        free(bwt);
    }
}
