
#include "vector.h"

#include <assert.h>
#include <math.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>

#include "memorypool.h"

// the number of elements that are reserved upon creating a vector
#define VECTOR_INITIAL_CAPACITY 4

// structure for the vector
struct vector
{
  int m_elemsize;             // the size of each element in the vector
  int m_count;                // the number of elements in the vector
  int m_capacity;             // the number of elements that can fit in the reserved memory
  struct memorypool *m_pool;  // the memory allocator
  void *m_data;               // a pointer to the vector data
};
  

// allocate a new vector
struct vector *_vector_alloc(int a_elemsize, struct memorypool *a_pool, const char *a_file, int a_line)
{
  // locals
  struct vector *ptr;

  // checks
  assert(a_elemsize);

  // allocate the vector structure memory
  ptr = (struct vector *) _palloc(a_file, a_line, a_pool, sizeof(struct vector));
  assert(ptr);

  // set the default values
  memset(ptr, 0, sizeof(struct vector));
  ptr->m_elemsize = a_elemsize;
  ptr->m_pool = a_pool;

  // reserve the initial capacity space
  _vector_reserve(ptr, VECTOR_INITIAL_CAPACITY, a_file, a_line);

  // return the vector
  return ptr;
}


// free a vector
void _vector_free(struct vector *a_vector)
{
  // checks
  assert(a_vector);

  // free the data and the vector
  pfree(a_vector->m_pool, a_vector->m_data);
  pfree(a_vector->m_pool, a_vector);
}


// reserve space for a number of elements
void _vector_reserve(struct vector *a_vector, int a_capacity, const char *a_file, int a_line)
{
  // locals
  void *ptr;

  // checks
  assert(a_vector);
  assert(a_capacity >= a_vector->m_count);

  // reallocate the requested space, which results in the old data being copied to the new data if 
  // the old data is not 0.
  ptr = _prealloc(a_file, a_line, a_vector->m_pool, a_vector->m_data, a_vector->m_elemsize * a_capacity);
  assert(ptr);

  // assign the data pointer and update the capacity
  a_vector->m_data = ptr;
  a_vector->m_capacity = a_capacity;
}


// set the number of elements in the vector
void _vector_resize(struct vector *a_vector, int a_count, const char *a_file, int a_line)
{
  // checks
  assert(a_vector);
  
  // if the required number of elements is greater than the capacity then reserve enough space
  if (a_count > a_vector->m_capacity)
  {
    _vector_reserve(a_vector, a_count, a_file, a_line);
  }

  // update the count
  a_vector->m_count = a_count;
}


// null the memory of all used elements in the vector
void _vector_zero(struct vector *a_vector)
{
  // checks
  assert(a_vector);

  // set the used memory to 0
  memset(a_vector->m_data, 0, a_vector->m_elemsize * a_vector->m_count);
}


// get a pointer to the first element in the vector
void *_vector_data(struct vector *a_vector)
{
  // checks
  assert(a_vector);

  // return the start of the vector data
  return a_vector->m_data;
}


// get a pointer to a specific element in the vector
void *_vector_index(struct vector *a_vector, int a_index)
{
  // locals
  char *ptr;

  // checks
  assert(a_vector);
  assert(a_index < a_vector->m_count);
  
  // get a pointer to the specified element
  ptr = (char *) a_vector->m_data + (a_vector->m_elemsize * a_index);
  return ptr;
}


// get the number of elements in the vector
int _vector_count(struct vector *a_vector)
{
  // checks
  assert(a_vector);

  // return the element count
  return a_vector->m_count;
}


// get the number of elements tha could fit in the allocated space in the vector
int _vector_capacity(struct vector *a_vector)
{
  // checks
  assert(a_vector);

  // return the capacity
  return a_vector->m_capacity;
}


// get the size of each element in the vector
int _vector_elemsize(struct vector *a_vector)
{
  // checks
  assert(a_vector);

  // return the element size
  return a_vector->m_elemsize;
}


// append an element to the vector
void *_vector_append(struct vector *a_vector, int a_count, const char *a_file, int a_line)
{
  // locals
  void *data;

  // checks
  assert(a_count > 0);
  assert(a_vector);

  // resize (and possible reallocate) the vector to the requested size
  _vector_resize(a_vector, a_vector->m_count + a_count, a_file, a_line);

  // return a pointer to start of the appended data
  data = vector_index(a_vector, a_vector->m_count - a_count);
  return data;
}


void *_vector_insert(struct vector *a_vector, int a_start, int a_count, const char *a_file, int a_line)
{
  // locals
  void *src, *dst;
  int movesize;

  // checks
  assert(a_count > 0);
  assert(a_start >= 0);
  assert(a_vector);
  assert(a_vector->m_count > 0);
  assert(a_start < a_vector->m_count);
  assert(a_start + a_count <= a_vector->m_count);

  // calculate the amount of memory after the insertion point
  movesize = a_vector->m_elemsize * (a_vector->m_count - a_start);

  // resize (and possibly reallocate) the vector to hold the new elements
  _vector_resize(a_vector, a_vector->m_count + a_count, a_file, a_line);

  // get a pointer to the data after the insertion point, and the place to copy it to
  src = vector_index(a_vector, a_start);
  dst = vector_index(a_vector, a_start + a_count);

  // move the data to create space for the inserted elements. note - memmove is used as it handles
  // copying overlapping memory.
  memmove(dst, src, movesize);
  return src;
}


// remove elements from the vector
void _vector_remove(struct vector *a_vector, int a_start, int a_count)
{
  // locals
  void *src, *dst;
  int movesize, src_idx, dst_idx;

  // checks
  assert(a_count > 0);
  assert(a_start >= 0);
  assert(a_vector);
  assert(a_vector->m_count > 0);
  assert(a_start < a_vector->m_count);
  assert(a_start + a_count <= a_vector->m_count);

  // get the index of the elements after deletion range and the index to copy them to
  src_idx = a_start + a_count;
  dst_idx = a_start;

  // get pointers for the indexes
  src = vector_index(a_vector, src_idx);
  dst = vector_index(a_vector, dst_idx);

  // calculate the size of the data to move
  movesize = a_vector->m_elemsize * (a_vector->m_count - src_idx);

  // move all data after the deletion range to the start of the deletion range
  memmove(dst, src, movesize);

  // update the number of elements in the vector
  a_vector->m_count -= a_count;
}


// -- EOF

