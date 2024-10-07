#include "move_table.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    size_t start_idx;
    uint8_t length;
    char head;
} run_t;

typedef struct {
    run_t *array;
    size_t length;
    size_t capacity;
} runs_array_t;

#define ARR_GROWTH_SIZE 2
#define ARR_INIT_SIZE 256

void _move_table_runs_array_extend(runs_array_t *ra) {
    assert(ra != NULL);

    size_t new_capacity = ra->capacity * ARR_GROWTH_SIZE;
    run_t *new_array = realloc(ra->array, new_capacity * sizeof(run_t));

    if (new_array == NULL) {
        perror("Error: could not allocate memory to grow run array.\n");
        exit(EXIT_FAILURE);
    }

    ra->array = new_array;
    ra->capacity = new_capacity;
}

void _move_table_run_array_insert(runs_array_t *ra, run_t run) {
    assert(ra != NULL);

    if (ra->length >= ra->capacity) {
        _move_table_runs_array_extend(ra);
    }

    ra->array[ra->length] = run;
    ra->length++;
}

runs_array_t *_move_table_runs_array_create() {
    runs_array_t *ra = malloc(sizeof(runs_array_t));
    ra->array = malloc(sizeof(run_t) * ARR_INIT_SIZE);
    ra->length = 0;
    ra->capacity = ARR_INIT_SIZE;

    if (ra->array == NULL) {
        perror("Error: could not allocate memory to create run array.\n");
        exit(EXIT_FAILURE);
    }

    return ra;
}

void _move_table_runs_array_destroy(runs_array_t *ra) {
    assert(ra != NULL);

    if (ra->array) {
        free(ra->array);
    }

    free(ra);
}

int _move_table_compare_runs(const void *a, const void *b) {
    const run_t *run_a = (const run_t *)a;
    const run_t *run_b = (const run_t *)b;
    return (int)(run_a->head - run_b->head);
}

void _move_table_sort_runs_by_head(runs_array_t *runs_array) {
    assert(runs_array != NULL);
    assert(runs_array->array != NULL);

    qsort(runs_array->array, runs_array->length, sizeof(run_t),
          _move_table_compare_runs);
}

/**
 * I've optimized this function for space and not speed.
 * This function does a linear search over the runs array to find
 * the corresponding index: O(r).
 *
 * This can be made faster using either another runs arrays that's
 * sorted on index rather than lexicographically. This would need O(r)
 * move space but search would take O(lg r).
 *
 * We can make it even faster using as hashmap, using O(n) space and
 * O(1) running time. n is the size of the bwt.
 */
size_t _move_table_runs_array_find(runs_array_t *ra, size_t idx) {
    assert(ra != NULL);

    for (size_t i = 0; i < ra->length; i++) {
        size_t sidx = ra->array[i].start_idx;
        uint8_t l = ra->array[i].length;

        if (idx >= sidx && idx < sidx + l) {
            return i;
        }
    }

    // should never reach here if used correctly
    assert(0);
    // something outside bounds
    return ra->length + 1;
}

move_table_t *move_table_create(char *bwt, size_t len) {
    assert(bwt != NULL && len > 0);

    runs_array_t *ra = _move_table_runs_array_create();

    run_t cur;
    cur.length = 1;
    cur.head = bwt[0];
    cur.start_idx = 0;

    for (size_t i = 1; i < len; i++) {
        if (cur.head == bwt[i]) {
            cur.length++;
            continue;
        }

        _move_table_run_array_insert(ra, cur);
        cur.length = 1;
        cur.head = bwt[i];
        cur.start_idx = i;
    }

    _move_table_run_array_insert(ra, cur);

    // sort runs lexicographically
    _move_table_sort_runs_by_head(ra);

    move_table_t *mt = malloc(sizeof(move_table_t));
    mt->table = malloc(sizeof(move_table_row_t) * ra->length);
    mt->length = ra->length;

    size_t bwt_idx = 0;
    for (size_t i = 0; i < ra->length; i++) {
        size_t ra_idx = _move_table_runs_array_find(ra, bwt_idx);
        mt->table[i].head = ra->array[ra_idx].head;
        mt->table[i].length = ra->array[ra_idx].length;
        mt->table[i].ptr = ra_idx;
        mt->table[i].offset = bwt_idx - ra->array[ra_idx].start_idx;

        // TODO: Very important note
        //  nate thinks ra->array[ra_idx].length
        //     I think  ra->array[i].length
        //  if we do ra_idx then due to the way offset is being calculated,
        //  offset will always be 0.
        bwt_idx += ra->array[ra_idx].length;
    }

    _move_table_runs_array_destroy(ra);

    return mt;
}

void move_table_print(move_table_t *mt) {
    if (mt == NULL || mt->table == NULL) {
        return;
    }

    printf("| chr | len |   ptr   | off |\n");
    printf("-----------------------------\n");
    for (size_t i = 0; i < mt->length; i++) {
        printf("| %3c | %3u | %7u | %3u |\n", mt->table[i].head,
               mt->table[i].length, mt->table[i].ptr, mt->table[i].offset);
    }
}

void move_table_store(move_table_t *mt, char *filename) {
    return;
}

move_table_t *move_table_load(char *filename) {
    return NULL;
}

void move_table_destroy(move_table_t *mt) {
    assert(mt != NULL);
    free(mt->table);
    free(mt);
}
