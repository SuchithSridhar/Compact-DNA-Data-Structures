#ifndef SS_VECTOR_H
#define SS_VECTOR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define GROWTH_FACTOR 2

typedef struct {
    /** The internal array container. */
    void *array;
    /** The current number of elements in the vector. */
    size_t size;
    /** The max capacity of the vector at a given time. */
    size_t capacity;
    /** The size of each element in the vector. */
    size_t memb_size;
} ssv_vector_t;

/**
 * Initialize a vector for elements of size memb_size.
 * @param memb_size the size of each element.
 * @return A new vector allocated on the heap.
 */
ssv_vector_t *ssv_init(size_t memb_size, size_t init_size);

/**
 * Destroy the vector and free its memory.
 * @param vec the ssv_vector_t to destroy.
 * @return true if the operation is successful, false otherwise.
 */
bool ssv_destroy(ssv_vector_t *vec);

/**
 * Reduces the capacity of the vector to match its current size, potentially
 * freeing up unused memory.
 * @param vec the ssv_vector_t to operate on.
 * @return true if the operation is successful and memory is potentially freed,
 * false otherwise.
 */
bool ssv_shrink_to_fit(ssv_vector_t *vec);

/**
 * Allocates memory to ensure the vector can hold at least a specified number of
 * elements before needing to resize. This can be beneficial for performance
 * when adding many elements to the vector.
 * @param vec the ssv_vector_t to operate on.
 * @param count the number of elements you want to reserve space for.
 * @return true if the operation is successful and memory is reserved, false
 * otherwise.
 */
bool ssv_reserve(ssv_vector_t *vec, size_t count);

/**
 * Creates and returns a new vector that is a copy of the provided vector. The
 * elements are also copied.
 * @param vec the ssv_vector_t to copy.
 * @return a new vector that is a copy of the original vector, or NULL if the
 * copy fails.
 */
ssv_vector_t *ssv_copy(ssv_vector_t *vec);

/**
 * Checks if the vector is empty (i.e., its size is 0).
 * @param vec the ssv_vector_t to check.
 * @return true if the vector is empty, false otherwise.
 */
bool ssv_is_empty(ssv_vector_t *vec);

/**
 * Deletes a range of elements from the vector, starting from the 'start' index
 * up to, but not including, the 'end' index. This operation will shift any
 * subsequent elements to fill the space created by the deleted elements.
 * @param vec the ssv_vector_t to operate on.
 * @param start the starting index of the range to delete.
 * @param end the ending index of the range to delete (exclusive).
 * @return the number of elements successfully deleted from the vector.
 */
size_t ssv_delete_range(ssv_vector_t *vec, size_t start, size_t end);

/**
 * Get an element from the vector at a specified index.
 * @param vec the ssv_vector_t to operate on.
 * @param index the index of the element to be retrieved.
 * @return a pointer within the vector to the element if found, NULL otherwise.
 */
void *ssv_get(ssv_vector_t *vec, size_t index);

/**
 * Push an element to the end of the vector.
 * @param vec the ssv_vector_t to operate on.
 * @param item the element to be added.
 * @return true if the operation is successful, false otherwise.
 */
bool ssv_push(ssv_vector_t *vec, void *item);

/**
 * Get an element from the end of the vector.
 * @param vec the ssv_vector_t to operate on.
 * @return a pointer within the vector to the element if exists, else NULL.
 */
void *ssv_peek(ssv_vector_t *vec);

/**
 * Pop an element from the end of the vector.
 * The returned element needs to be freed.
 * @param vec the ssv_vector_t to operate on.
 * @return a pointer to the popped element (on heap), NULL if fails.
 */
void *ssv_pop(ssv_vector_t *vec);

/**
 * Clear all the elements of the array.
 * Note that this only updated the size
 * and does not actually clear the array.
 * @param vec the ssv_vector_t to clear.
 * @return true if operation is successful, false otherwise.
 */
bool ssv_clear(ssv_vector_t *vec);

/**
 * Delete an element from the end of the vector.
 * @param vec the ssv_vector_t to operate on.
 * @return true if operation is successful, false otherwise.
 */
bool ssv_delete(ssv_vector_t *vec);

/**
 * Delete an element at the specified index.
 * @param index the index at which the element is to be deleted.
 * @param vec the ssv_vector_t to operate on.
 * @return true if operation is successful, false otherwise.
 */
bool ssv_delete_at(ssv_vector_t *vec, size_t index);

/**
 * Pop an element from the vector at a specified index.
 * @param vec the ssv_vector_t to operate on.
 * @param index the index at which the element is to be popped.
 * @return a pointer to the popped element, NULL otherwise.
 */
void *ssv_pop_at(ssv_vector_t *vec, size_t index);

/**
 * Push an element into the vector at a specified index.
 * @param vec the ssv_vector_t to operate on.
 * @param item the element to be inserted.
 * @param index the index at which the element is to be inserted.
 * @return true if the operation is successful, false otherwise.
 */
bool ssv_push_at(ssv_vector_t *vec, void *item, size_t index);

/**
 * Replace an element in the vector at a specified index with another element.
 * @param vec the ssv_vector_t to operate on.
 * @param item the new element to insert.
 * @param index the index of the element to be replaced.
 * @return true if the operation is successful, false otherwise.
 */
bool ssv_replace_at(ssv_vector_t *vec, void *item, size_t index);

/**
 * Find the index of an element in the vector.
 * @param vec the ssv_vector_t to operate on.
 * @param item the element to search for.
 * @return the index of the element if found, SIZE_MAX otherwise.
 */
size_t ssv_index_of(ssv_vector_t *vec, void *item);

/**
 * Print the elements inside the vector.
 * @param vec the ssv_vector_t to operate on.
 * @param print_elem a function pointer to print an element.
 */
void ssv_print(ssv_vector_t *vec, void (*print_elem)(void *));

/**
 * Sort the vector based on the provided compare function.
 * @param vec the ssv_vector_t to operate on.
 * @param compare a function point to a function to compare two elements.
 */
void ssv_sort(ssv_vector_t *vec, int (*compare)(const void *, const void *));

bool _internal_ssv_resize(ssv_vector_t *vec);

#endif
