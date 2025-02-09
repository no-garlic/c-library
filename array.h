
#ifndef __h_array
#define __h_array

#ifdef  __cplusplus
extern "C" {
#endif // __cplusplus

#include "config.h"

// forward declarations
typedef struct array;
typedef struct memorypool;

// allocate a new array
// a_elemsize the size of each element in the array
// a_count the number of elements to allocate
// a_pool the memory pool to allocate from
// returns the newly allocated array
#define array_alloc(a_elemsize, a_count, a_pool) _array_alloc(a_elemsize, a_count, a_pool, __FILE__, __LINE__)

// free the array and all allocated memory
// a_array the array to operate on
#define array_free(a_array) _array_free(a_array)

// resize the array
// a_array the array to operate on
// a_count the number of elements for the array to hold
#define array_resize(a_array, a_count) _array_resize(a_array, a_count, __FILE__, __LINE__)

// get the element count of the array
// a_array the array to operate on
// returns the number of elements in the array
#define array_count(a_array) _array_count(a_array)

// get the element size of the array
// a_array the array to operate on
// returns the size of each element in the array
#define array_elemsize(a_array) _array_elemsize(a_array)

// zero out all elements in the array
// a_array the array to operate on
#define array_zero(a_array) _array_zero(a_array)

// gets a pointer to the memory for the array
// a_array the array to operate on
// returns a pointer to the first element
#define array_data(a_array) _array_data(a_array)

// gets a pointer to the memory for an element in the array
// a_array the array to operate on
// aindex the index of the element to get a pointer to
// returns a pointer to the element
#define array_index(a_array, a_index) _array_index(a_array, a_index)

// interface functions
struct array *_array_alloc(int a_elemsize, int a_count, struct memorypool *a_pool, const char *a_file, int a_line);
void _array_free(struct array *a_array);
void _array_resize(struct array *a_array, int a_count, const char *a_file, int a_line);
int _array_count(struct array *a_array);
int _array_elemsize(struct array *a_array);
void _array_zero(struct array *a_array);
void *_array_data(struct array *a_array);
void *_array_index(struct array *a_array, int a_index);

#ifdef  __cplusplus
}
#endif // __cplusplus

#endif // __h_array

// -- EOF

