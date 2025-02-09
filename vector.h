
#ifndef __h_vector
#define __h_vector

#ifdef  __cplusplus
extern "C" {
#endif // __cplusplus

#include "config.h"

// forward declarations
typedef struct vector;
typedef struct memorypool;

// allocate a new vector
// a_elemsize the size of each element in the vector
// a_pool the memory pool to allocate from
// returns the newly allocated vector
#define vector_alloc(a_elemsize, a_pool) _vector_alloc(a_elemsize, a_pool, __FILE__, __LINE__)

// free the vector and all allocated memory
// a_vector the vector to operate on
#define vector_free(a_vector) _vector_free(a_vector)

// reserve memory within the vector for expansion
// a_vector the vector to operate on
// a_capacity the number of elements to reserve memory for
#define vector_reserve(a_vector, a_capacity) _vector_reserve(a_vector, a_capacity, __FILE__, __LINE__)

// resize the vector
// a_vector the vector to operate on
// a_count the number of elements to insert into the vector
#define vector_resize(a_vector, a_count) _vector_resize(a_vector, a_count, __FILE__, __LINE__)

// get the element count of the vector
// a_vector the vector to operate on
// returns the number of elements in the vector
#define vector_count(a_vector) _vector_count(a_vector)

// get the (reserved) element capacity of the vector
// a_vector the vector to operate on
// returns the number elements that can fit in the reserved memory
#define vector_capacity(a_vector) _vector_capacity(a_vector)

// get the element size of the vector
// a_vector the vector to operate on
// returns the size of each element in the vector
#define vector_elemsize(a_vector) _vector_elemsize(a_vector)

// zero out all elements in the vector
// a_vector the vector to operate on
#define vector_zero(a_vector) _vector_zero(a_vector)

// append new elements at the end of the vector
// a_vector the vector to operate on
// a_count the number of elements to add
// returns a pointer to the memory at start of the first element
#define vector_append(a_vector, a_count) _vector_append(a_vector, a_count, __FILE__, __LINE__)

// insert an element into the vector
// a_vector the vector to operate on
// a_start the index to insert the elements after in the vector
// a_count the number of elements to add
// returns a pointer to the memory at start of the first element
#define vector_insert(a_vector, a_start, a_count) _vector_insert(a_vector, a_start, a_count, __FILE__, __LINE__)

// removes elements from the vector
// a_vector the vector to operate on
// a_start the index of the fist element to remove
// a_count the number of elements to remove
#define vector_remove(a_vector, a_start, a_count) _vector_remove(a_vector, a_start, a_count)

// gets a pointer to the memory for the vector
// a_vector the vector to operate on
// returns a pointer to the first element
#define vector_data(a_vector) _vector_data(a_vector)

// gets a pointer to the memory for an element in the vector
// a_vector the vector to operate on
// aindex the index of the element to get a pointer to
// returns a pointer to the element
#define vector_index(a_vector, a_index) _vector_index(a_vector, a_index)

// interface functions
struct vector *_vector_alloc(int a_elemsize, struct memorypool *a_pool, const char *a_file, int a_line);
void _vector_free(struct vector *a_vector);
void _vector_reserve(struct vector *a_vector, int a_capacity, const char *a_file, int a_line);
void _vector_resize(struct vector *a_vector, int a_count, const char *a_file, int a_line);
int _vector_count(struct vector *a_vector);
int _vector_capacity(struct vector *a_vector);
int _vector_elemsize(struct vector *a_vector);
void _vector_zero(struct vector *a_vector);
void *_vector_append(struct vector *a_vector, int a_count, const char *a_file, int a_line);
void *_vector_insert(struct vector *a_vector, int a_start, int a_count, const char *a_file, int a_line);
void _vector_remove(struct vector *a_vector, int a_start, int a_count);
void *_vector_data(struct vector *a_vector);
void *_vector_index(struct vector *a_vector, int a_index);

#ifdef  __cplusplus
}
#endif // __cplusplus

#endif // __h_vector

// -- EOF

