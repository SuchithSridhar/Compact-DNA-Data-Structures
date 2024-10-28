#include "ssvector.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ssv_vector_t *ssv_init(size_t memb_size, size_t init_size) {
    ssv_vector_t *vec = malloc(sizeof(ssv_vector_t));

    if (vec == NULL)
        return NULL;

    vec->size = 0;
    vec->memb_size = memb_size;
    vec->capacity = init_size;

    vec->array = malloc(memb_size * init_size);
    if (vec->array == NULL) {
        free(vec);
        return NULL;
    }

    return vec;
}

bool ssv_destroy(ssv_vector_t *vec) {
    if (vec == NULL)
        return true;

    free(vec->array);
    free(vec);

    return true;
}

bool ssv_shrink_to_fit(ssv_vector_t *vec) {
    if (vec == NULL)
        return false;

    if (vec->size == vec->capacity)
        return true;

    void *new_array = realloc(vec->array, vec->size * vec->memb_size);
    if (new_array) {
        vec->array = new_array;
        vec->capacity = vec->size;
        return true;
    }
    return false;
}

bool ssv_reserve(ssv_vector_t *vec, size_t count) {
    if (vec == NULL)
        return false;
    // Not allowed to reduce the capacity below size.
    if (count <= vec->size)
        return false;
    // We already have the required capacity.
    if (count <= vec->capacity)
        return true;

    void *new_array = realloc(vec->array, count * vec->memb_size);
    if (new_array) {
        vec->array = new_array;
        vec->capacity = count;
        return true;
    }
    return false;
}

ssv_vector_t *ssv_copy(ssv_vector_t *vec) {
    if (vec == NULL)
        return NULL;

    ssv_vector_t *new_vec = malloc(sizeof(ssv_vector_t));

    if (!new_vec)
        return NULL;

    new_vec->array = malloc(vec->capacity * vec->memb_size);
    if (!new_vec->array) {
        free(new_vec);
        return NULL;
    }

    memcpy(new_vec->array, vec->array, vec->size * vec->memb_size);
    new_vec->size = vec->size;
    new_vec->capacity = vec->capacity;
    new_vec->memb_size = vec->memb_size;
    return new_vec;
}

bool ssv_is_empty(ssv_vector_t *vec) {
    return vec && vec->size == 0;
}

size_t ssv_delete_range(ssv_vector_t *vec, size_t start, size_t end) {
    if (!vec || start >= end || end > vec->size) {
        return 0;
    }
    size_t range = end - start;
    void *start_ptr = (char *)vec->array + start * vec->memb_size;
    void *end_ptr = (char *)vec->array + end * vec->memb_size;
    size_t move_count = vec->size - end;
    memmove(start_ptr, end_ptr, move_count * vec->memb_size);
    vec->size -= range;
    return range;
}

void *ssv_get(ssv_vector_t *vec, size_t index) {
    if (vec && index < vec->size) {
        return (void *)((char *)vec->array + (index * vec->memb_size));
    }

    return NULL;
}

void *ssv_peek(ssv_vector_t *vec) {
    if (vec && vec->size > 0) {
        return (void *)((char *)vec->array +
                        ((vec->size - 1) * vec->memb_size));
    }

    return NULL;
}

bool ssv_push(ssv_vector_t *vec, void *item) {
    if (vec == NULL || item == NULL)
        return false;
    bool success = true;
    if (vec->size == vec->capacity) {
        success = _internal_ssv_resize(vec);
        if (!success)
            return false;
    }

    void *dest = (char *)vec->array + (vec->memb_size * vec->size);
    memcpy(dest, item, vec->memb_size);
    vec->size++;

    return true;
}

void *ssv_pop(ssv_vector_t *vec) {
    if (vec == NULL || vec->size == 0)
        return NULL;

    void *src = (char *)vec->array + (vec->memb_size * (vec->size - 1));
    void *elem = malloc(vec->memb_size);

    memcpy(elem, src, vec->memb_size);
    vec->size--;
    return elem;
}

bool ssv_clear(ssv_vector_t *vec) {
    if (vec == NULL)
        return false;
    vec->size = 0;
    return true;
}

bool ssv_delete(ssv_vector_t *vec) {
    if (vec == NULL || vec->size == 0)
        return false;
    vec->size--;
    return true;
}

bool ssv_delete_at(ssv_vector_t *vec, size_t index) {
    if (vec == NULL || vec->size == 0 || index >= vec->size) {
        return false;
    }

    void *to_delete = (char *)vec->array + (index * vec->memb_size);

    if (index < vec->size - 1) {
        void *start = (char *)to_delete + vec->memb_size;
        size_t bytes_to_move = (vec->size - index - 1) * vec->memb_size;
        memmove(to_delete, start, bytes_to_move);
    }
    vec->size--;

    return true;
}

void *ssv_pop_at(ssv_vector_t *vec, size_t index) {
    if (vec == NULL || vec->size == 0 || index >= vec->size) {
        return NULL;
    }

    void *elem = malloc(vec->memb_size);
    void *to_delete = (char *)vec->array + (index * vec->memb_size);

    memcpy(elem, to_delete, vec->memb_size);

    if (index < vec->size - 1) {
        void *start = (char *)to_delete + vec->memb_size;
        size_t bytes_to_move = (vec->size - index - 1) * vec->memb_size;
        memmove(to_delete, start, bytes_to_move);
    }
    vec->size--;
    return elem;
}

bool ssv_push_at(ssv_vector_t *vec, void *item, size_t index) {
    if (!vec || index > vec->size) {
        return false;
    }

    if (vec->size == vec->capacity) {
        bool success = _internal_ssv_resize(vec);
        if (!success)
            return false;
    }

    if (index < vec->size) {
        void *start = (char *)vec->array + (index * vec->memb_size);
        size_t bytes_to_move = (vec->size - index) * vec->memb_size;
        memmove((char *)start + vec->memb_size, start, bytes_to_move);
    }

    void *destination = (char *)vec->array + (index * vec->memb_size);
    memcpy(destination, item, vec->memb_size);

    vec->size++;

    return true;
}

bool ssv_replace_at(ssv_vector_t *vec, void *item, size_t index) {
    if (!vec || index > vec->size) {
        return false;
    }

    void *start = (char *)vec->array + (index * vec->memb_size);
    memcpy(start, item, vec->memb_size);
    return true;
}

size_t ssv_index_of(ssv_vector_t *vec, void *item) {
    if (vec == NULL || item == NULL)
        return SIZE_MAX;

    void *elem;
    for (size_t i = 0; i < vec->size; i++) {
        elem = (char *)vec->array + (vec->memb_size * i);
        if (memcmp(item, elem, vec->memb_size) == 0) {
            return i;
        }
    }
    return SIZE_MAX;
}

void ssv_print(ssv_vector_t *vec, void (*print_elem)(void *)) {
    for (size_t i = 0; i < vec->size; i++) {
        print_elem((char *)vec->array + (i * vec->memb_size));
    }
}

void ssv_sort(ssv_vector_t *vec, int (*compare)(const void *, const void *)) {
    qsort(vec->array, vec->size, vec->memb_size, compare);
}

bool _internal_ssv_resize(ssv_vector_t *vec) {
    if (vec == NULL)
        return false;
    size_t new_size = vec->capacity * GROWTH_FACTOR;

    // Something went wrong. Overflow?
    if (new_size < vec->capacity)
        return false;

    // Rounding cause the size to remain the same,
    // it should increase at least by 1.
    // This also accounts for if capacity is 0. It will update to be 1.
    if (new_size == vec->capacity) {
        new_size = new_size + 1;
    }

    void *new_array = realloc(vec->array, new_size * vec->memb_size);
    if (new_array == NULL)
        return false;

    vec->array = new_array;
    vec->capacity = new_size;
    return true;
}
