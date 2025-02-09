
#include "array.h"

#include <assert.h>
#include <memory.h>
#include <stdlib.h>

#include "memorypool.h"

// structure for the array
struct array
{
  int m_elemsize;             // the size of each element in the vector
  int m_count;                // the number of elements in the vector
  struct memorypool *m_pool;  // the memory allocator
  void *m_data;               // a pointer to the array data
};
  

// allocate a new array
struct array *_array_alloc(int a_elemsize, int a_count, struct memorypool *a_pool, const char *a_file, int a_line)
{
  // locals
  struct array *ptr;

  // checks
  assert(a_elemsize);

  // allocate the array structure memory
  ptr = (struct array *) _palloc(a_file, a_line, a_pool, sizeof(struct array));
  assert(ptr);

  // set the default values
  ptr->m_elemsize = a_elemsize;
  ptr->m_count = a_count;
  ptr->m_pool = a_pool;
  ptr->m_data = 0;

  // allocate the array data
  if (a_elemsize * a_count > 0)
  {
    ptr->m_data = _palloc(a_file, a_line, a_pool, a_elemsize * a_count);
    assert(ptr->m_data);
  }

  // return a pointer to the array
  return ptr;
}


// free an array
void array_free(struct array *a_array)
{
  // checks
  assert(a_array);

  // free the data and the array
  pfree(a_array->m_pool, a_array->m_data);
  pfree(a_array->m_pool, a_array);
}


// resize the array memory
void _array_resize(struct array *a_array, int a_count, const char *a_file, int a_line)
{
  // locals
  void *ptr;
  
  // checks
  assert(a_array);

  // check the new size of the array
  if (a_count == 0)
  {
    // if the size is 0 then free the current data
    pfree(a_array->m_pool, a_array->m_data);

    // set the data and count to 0
    a_array->m_data = 0;
    a_array->m_count = 0;
  }
  else
  {
    // if the size is not 0 then reallocate the memory
    ptr = _prealloc(a_file, a_line, a_array->m_pool, a_array->m_data, a_array->m_elemsize * a_count);
    assert(ptr);

    // store the count and a pointer to the new data
    a_array->m_data = ptr;
    a_array->m_count = a_count;
  }
}


// null all memory in the array
void array_zero(struct array *a_array)
{
  // checks
  assert(a_array);

  // set the array memory to 0
  memset(a_array->m_data, 0, a_array->m_elemsize * a_array->m_count);
}


// get a pointer to the data
void *array_data(struct array *a_array)
{
  // checks
  assert(a_array);

  // return the start of the data
  return a_array->m_data;
}


// get a pointer to an element in the array
void *array_index(struct array *a_array, int a_index)
{
  // locals
  char *ptr;

  // checks
  assert(a_array);
  assert(a_index < a_array->m_count);
  
  // return a pointer to the element
  ptr = (char *) a_array->m_data + (a_array->m_elemsize * a_index);
  return ptr;
}


// get the number of elements in the array
int array_count(struct array *a_array)
{
  // checks
  assert(a_array);

  // return the element count
  return a_array->m_count;
}


// get the size of each element in the array
int array_elemsize(struct array *a_array)
{
  // checks
  assert(a_array);

  // return the element size
  return a_array->m_elemsize;
}


// -- EOF

