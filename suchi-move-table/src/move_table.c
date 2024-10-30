#include "move_table.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

typedef struct {
    size_t ra_idx;
    size_t bwt_idx;
} ra_find_return;

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

runs_array_t *_move_table_runs_array_copy(runs_array_t *ra) {
    assert(ra != NULL);
    assert(ra->array != NULL);

    runs_array_t *ra_copy = malloc(sizeof(runs_array_t));
    if (ra_copy == NULL) {
        perror("Error: could not allocate memory for runs array structure.\n");
        exit(EXIT_FAILURE);
    }

    ra_copy->array = malloc(sizeof(run_t) * ra->length);
    if (ra_copy->array == NULL) {
        perror("Error: could not allocate memory to create run array.\n");
        free(ra_copy);
        exit(EXIT_FAILURE);
    }

    memcpy(ra_copy->array, ra->array, sizeof(run_t) * ra->length);

    ra_copy->length = ra->length;
    ra_copy->capacity = ra->length;

    return ra_copy;
}

// Function to print a single run_t element
void _move_table_run_print(const run_t *run) {
    printf("Run { head: '%c', length: %u, start_idx: %zu }\n", run->head,
           run->length, run->start_idx);
}

void _move_table_runs_array_print(runs_array_t *ra) {
    printf("Runs Array (length: %zu, capacity: %zu):\n", ra->length,
           ra->capacity);
    for (size_t i = 0; i < ra->length; ++i) {
        printf("  [%zu] ", i);
        _move_table_run_print(&ra->array[i]);
    }
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
ra_find_return _move_table_runs_array_find(runs_array_t *ra, size_t idx) {
    assert(ra != NULL);

    size_t sum = 0;
    for (size_t i = 0; i < ra->length; i++) {
        size_t sidx = ra->array[i].start_idx;
        uint8_t l = ra->array[i].length;

        if (idx >= sidx && idx < sidx + l) {
            return (ra_find_return){.ra_idx = i, .bwt_idx = sum};
        }

        sum += ra->array[i].length;
    }

    // should never reach here if used correctly
    printf("idx: %zu", idx);
    assert(0);
    // something outside bounds
    return (ra_find_return){.ra_idx = 0, .bwt_idx = 0};
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
    runs_array_t *ra_sorted = _move_table_runs_array_copy(ra);
    _move_table_sort_runs_by_head(ra_sorted);

    assert(ra_sorted != NULL);
    assert(ra_sorted->array != NULL);
    assert(ra_sorted->length == ra->length);

    // printf(">>> INITAL RUNS ARRAY:\n");
    // _move_table_runs_array_print(ra);

    // printf(">>> SORTED RUNS ARRAY:\n");
    // _move_table_runs_array_print(ra_sorted);

    move_table_t *mt = malloc(sizeof(move_table_t));
    mt->table = malloc(sizeof(move_table_row_t) * ra->length);
    mt->length = ra->length;

    size_t bwt_idx = 0;
    for (size_t i = 0; i < ra->length; i++) {

        ra_find_return raf =
            _move_table_runs_array_find(ra_sorted, ra->array[i].start_idx);
        ra_find_return ral = _move_table_runs_array_find(ra, raf.bwt_idx);

        mt->table[i].head = ra->array[i].head;
        mt->table[i].length = ra->array[i].length;
        mt->table[i].ptr = ral.ra_idx;
        mt->table[i].offset = raf.bwt_idx - ra->array[ral.ra_idx].start_idx;

        bwt_idx += ra->array[i].length;
    }

    _move_table_runs_array_destroy(ra);
    _move_table_runs_array_destroy(ra_sorted);

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

void move_table_store(move_table_t *mt, char *filename, size_t bwt_len) {
    FILE *file = fopen(filename, "wb");
	rewind(file);

	fwrite(&bwt_len, sizeof(size_t), 1, file);
	fwrite(&mt->length, sizeof(size_t), 1, file);

	for (int i = 0; i < mt->length; i++) {

		fwrite(&mt->table[i].head, 1, 1, file);
		fwrite(&mt->table[i].length, 1, 1, file);
		fwrite(&mt->table[i].ptr, sizeof(uint32_t), 1, file);
		fwrite(&mt->table[i].offset, 1, 1, file);
	}
    
    fclose(file);
    return;
}

move_table_t *move_table_load(char *filename) {
    FILE *file = fopen(filename, "rb");
	rewind(file);

    size_t bwt_len;
    size_t num_runs;

	fread(&bwt_len, sizeof(size_t), 1, file);
	fprintf(stderr, "read n = %i\n", bwt_len);

	fread(&num_runs, sizeof(size_t), 1, file);
	fprintf(stderr, "read r = %i\n", num_runs);

    move_table_t *mt = malloc(sizeof(move_table_t));
    mt->table = malloc(sizeof(move_table_row_t) * num_runs);
    mt->length = num_runs;

	for (int i = 0; i < num_runs; i++) {

        fwrite(&mt->table[i].head, 1, 1, file);
		fwrite(&mt->table[i].length, 1, 1, file);
		fwrite(&mt->table[i].ptr, sizeof(uint32_t), 1, file);
		fwrite(&mt->table[i].offset, 1, 1, file);
	}

	fprintf(stderr, "read table\n");

	fclose(file);
}

void move_table_destroy(move_table_t *mt) {
    assert(mt != NULL);
    free(mt->table);
    free(mt);
}
