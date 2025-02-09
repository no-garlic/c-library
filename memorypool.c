
#include "memorypool.h"

#include <assert.h>
#include <crtdbg.h>
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#include "array.h"


#ifndef __h_config_memorypool_enabled
#error config.h has not been included.
#endif


#define OS_ALLOC(a_size) malloc(a_size)
#define OS_FREE(a_ptr) free(a_ptr)
#define OS_REALLOC(a_ptr, a_size) realloc(a_ptr, a_size)

#define ALLOC_HEADER 0x1f4b0d2a
#define ALLOC_FOOTER 0x81fb3a92

#define HEAP_ALLOC_INDEX 0xffffffff

#define CHUNK_ITEMS 32


struct heapchunk
{
  struct heapchunk *m_next;
  struct heapchunk *m_prev;
  int m_size;
  
  int m_header;

#if __h_config_memory_pool_tracking
  const char *m_file;
  int m_line;
#endif

  int m_index; // will equal ~0 for a heapchunk
  struct heapchunk *m_chunk;

  int m_end;
};


struct memorychunk
{
  int m_size;
  int m_mask;
  struct memorychunk *m_next;

  // struct data
  // {
  //   int m_header
  //if __h_config_memory_pool_tracking
  //   const char *m_file;
  //   int m_line;
  //endif
  //   int m_index; // will be >=0 and <32 for a memorychunk
  //   struct memorychunk *m_chunk;
  //   char m_data[size];
  //   int m_footer
  // } m_data[CHUNK_ITEMS];
};


enum
{
  chunk_8     = 0,
  chunk_16    = 1,     
  chunk_32    = 2,
  chunk_48    = 3,
  chunk_64    = 4,
  chunk_128   = 5,
  chunk_256   = 6,
  chunk_count = 7
};


uint32 g_memorychunk_size[chunk_count] = 
{
  8,
  16,
  32,
  48,
  64,
  128,
  256
};


struct memorypool
{
  struct memorychunk *m_chunks[chunk_count];
  struct heapchunk *m_heap;
  int m_num_alloc;
};


struct memorypool *g_global_memorypool = 0;


void *__alloc_from_heap(const char *a_file, int a_line, struct memorypool *a_pool, uint32 a_size);
void *__alloc_from_chunk(const char *a_file, int a_line, struct memorypool *a_pool, int a_id);
int __alloc_chunk_size(int a_id);
struct memorychunk *__alloc_new_chunk(struct memorypool *a_pool, int a_id);
void __free_chunk(struct memorychunk *a_chunk);
void __free_heap(struct memorypool *a_pool, struct heapchunk *a_chunk);
void *__chunk_item_data(struct memorychunk *a_chunk, int a_index);
int *__chunk_item_header(struct memorychunk *a_chunk, int a_index);
int *__chunk_item_footer(struct memorychunk *a_chunk, int a_index);
void __show_pool_allocations(struct memorypool *a_pool);
void __show_chunk_allocations(struct memorychunk *a_chunk);
void __show_heap_allocations(struct heapchunk *a_chunk);
void __show_allocation(const char *a_source, const char *a_file, int a_line, int a_size);


void _memorypool_global(struct memorypool *a_pool)
{
  g_global_memorypool = a_pool;
}


struct memorypool *_memorypool_alloc()
{
  struct memorypool *pool;

  _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);

  pool = OS_ALLOC(sizeof(struct memorypool));
  assert(pool);

  memset(pool, 0, sizeof(struct memorypool));
  return pool;
}


void _memorypool_trunc(struct memorypool *a_pool)
{
  int i;
  struct memorychunk *chunk;

  assert(a_pool);
  
  for (i = 0; i < chunk_count; i++)
  {
    chunk = a_pool->m_chunks[i];

    if (chunk)
    {
      __free_chunk(chunk);
    }
  }

  while (a_pool->m_heap)
  {
    __free_heap(a_pool, a_pool->m_heap);
  }

  memset(a_pool, 0, sizeof(struct memorypool));
}


int _memorypool_free(struct memorypool *a_pool)
{
  int i;
  struct memorychunk *chunk;

  assert(a_pool);
  
  if (a_pool->m_num_alloc > 0)
  {
    __show_pool_allocations(a_pool);
    return -1;
  }

  if (a_pool == g_global_memorypool)
  {
    g_global_memorypool = 0;
  }

  for (i = 0; i < chunk_count; i++)
  {
    chunk = a_pool->m_chunks[i];

    if (chunk)
    {
      __free_chunk(chunk);
    }
  }

  while (a_pool->m_heap)
  {
    __free_heap(a_pool, a_pool->m_heap);
  }

  OS_FREE(a_pool);
  return 0;
}


void _memorypool_report(struct memorypool *a_pool)
{
  if (a_pool->m_num_alloc > 0)
  {
    __show_pool_allocations(a_pool);
  }
}


void *_palloc(const char *a_file, int a_line, struct memorypool *a_pool, uint32 a_size)
{
#if __h_config_memorypool_enabled
  void *mem;
  int i;

  assert(chunk_count);

  if (!a_pool)
  {
    a_pool = g_global_memorypool;
  }

  if (a_pool)
  {
    if (a_size > g_memorychunk_size[chunk_count - 1])
    {
      mem = __alloc_from_heap(a_file, a_line, a_pool, a_size);
    }
    else
    {
      for (i = 0; i < chunk_count; i++)
      {
        if (a_size <= g_memorychunk_size[i])
        {
          mem = __alloc_from_chunk(a_file, a_line, a_pool, i);
          break;
        }
      }
    }

    if (mem)
    {
      a_pool->m_num_alloc++;
    }
  }
  else
  {
    mem = 0;
  }

  return mem;
#else
  return OS_ALLOC(a_size);
#endif
}


// todo: make each chunk point to the pool so realloc does not need to specify the pool
void *_prealloc(const char *a_file, int a_line, struct memorypool *a_pool, void *a_mem, uint32 a_size)
{
#if __h_config_memorypool_enabled
  struct memorychunk *chunk;
  struct heapchunk *hchunk;
  int index, *ptr;
  uint32 oldsize;
  void *newmem;

  if (!a_pool)
  {
    a_pool = g_global_memorypool;
  }

  assert(a_pool);
  assert(a_size >= 0);

  if (a_mem)
  {
#if __h_config_memory_pool_tracking
    ptr = (int *) a_mem;
    ptr -= 5;
    assert(ptr[0] == ALLOC_HEADER);
    ptr += 2;
#else
    ptr = (int *) a_mem;
    ptr -= 3;
    assert(ptr[0] == ALLOC_HEADER);
#endif

    index = ptr[1];
    chunk = (struct memorychunk *) ptr[2];

    if (index == HEAP_ALLOC_INDEX)
    {
      hchunk = (struct heapchunk *) chunk;
      oldsize = hchunk->m_size;
    }
    else
    {
      assert(index >= 0 && index < 32);
      oldsize = chunk->m_size;
    }
    
    if (oldsize >= a_size)
    {
      return a_mem;
    }
  }
  else
  {
    oldsize = 0;
  }

  newmem = _palloc(a_file, a_line, a_pool, a_size);
  assert(newmem);

  if (oldsize)
  {
    memcpy(newmem, a_mem, min(a_size, oldsize));
   _pfree(a_pool, a_mem);
  }

  return newmem;

#else
  return OS_REALLOC(a_mem, a_size);
#endif  
}


// todo: make each chunk point to the pool so free does not need to specify the pool
void pfree(struct memorypool *a_pool, void *a_mem)
{
#if __h_config_memorypool_enabled
  int mask, index, *ptr;
  struct memorychunk *chunk;
  struct heapchunk *hchunk;
  
  if (!a_pool)
  {
    a_pool = g_global_memorypool;
  }

  assert(a_pool);
  assert(a_pool->m_num_alloc > 0);

  a_pool->m_num_alloc--;

#if __h_config_memory_pool_tracking
  ptr = (int *) a_mem;
  ptr -= 5;
  assert(ptr[0] == ALLOC_HEADER);
  ptr += 2;
#else
  ptr = (int *) a_mem;
  ptr -= 3;
  assert(ptr[0] == ALLOC_HEADER);
#endif

  index = ptr[1];
  chunk = (struct memorychunk *) ptr[2];

  if (index == HEAP_ALLOC_INDEX)
  {
    hchunk = (struct heapchunk *) chunk;
    __free_heap(a_pool, hchunk);
  }
  else
  {
    assert(chunk);
    assert(chunk->m_size);
    assert(index >= 0 && index < CHUNK_ITEMS);

    ptr = __chunk_item_footer(chunk, index);
    assert(ptr[0] == ALLOC_FOOTER);

    mask = 1 << index;
    chunk->m_mask &= ~mask;
  }
#else
  OS_FREE(a_mem);
#endif
}


void *__alloc_from_chunk(const char *a_file, int a_line, struct memorypool *a_pool, int a_id)
{
  struct memorychunk *chunk;
  int size, i, mask;
  int *ptr;

  assert(a_pool);

  chunk = a_pool->m_chunks[a_id];

  while (!chunk || chunk->m_mask == 0xffffffff)
  {
    if (chunk)
    {
      chunk = chunk->m_next;
    }

    if (!chunk)
    {
      chunk = __alloc_new_chunk(a_pool, a_id);
      assert(chunk);

      size = __alloc_chunk_size(a_id);
      memset(chunk, 0, size);

      chunk->m_next = a_pool->m_chunks[a_id];
      a_pool->m_chunks[a_id] = chunk;

      chunk->m_size = g_memorychunk_size[a_id];
    }
  }

  assert(chunk);
  for (i = 0; i < CHUNK_ITEMS; i++)
  {
    mask = 1 << i;
    if (((mask & chunk->m_mask) == 0))
    {
      chunk->m_mask |= mask;

      ptr = __chunk_item_header(chunk, i);
      ptr[0] = ALLOC_HEADER;

#if __h_config_memory_pool_tracking
      ptr[1] = (int) (int *) a_file;
      ptr[2] = a_line;
      ptr += 2;
#endif

      ptr[1] = i;
      ptr[2] = (int) (int *) chunk;

      ptr = __chunk_item_footer(chunk, i);
      ptr[0] = ALLOC_FOOTER;

      ptr = __chunk_item_data(chunk, i);
      return ptr;
    }
  }

  return 0;
}


void *__alloc_from_heap(const char *a_file, int a_line, struct memorypool *a_pool, uint32 a_size)
{
  int allocsize;
  void *mem;
  int *ptr;
  char *ptrchar;
  struct heapchunk *chunk;

  allocsize = a_size + sizeof(struct heapchunk);

  mem = OS_ALLOC(allocsize);
  assert(mem);

  ptr = (int *) mem;
  chunk = (struct heapchunk *) mem;

  chunk->m_chunk = chunk;
  chunk->m_header = ALLOC_HEADER;
  chunk->m_size = (int) a_size;
  chunk->m_index = HEAP_ALLOC_INDEX;

  chunk->m_next = a_pool->m_heap;
  chunk->m_prev = 0;
  if (a_pool->m_heap)
  {
    a_pool->m_heap->m_prev = chunk;
  }
  a_pool->m_heap = chunk;

#if __h_config_memory_pool_tracking
  chunk->m_file = a_file;
  chunk->m_line = a_line;
#endif

  mem = &chunk->m_end;

  ptrchar = (char *) mem;
  ptrchar += a_size;
  ptr = (int *) ptrchar;
  ptr[0] = ALLOC_FOOTER;

  return mem;
}


int __alloc_chunk_size(int a_id)
{
  int size;

  size = sizeof(struct memorychunk) + (CHUNK_ITEMS * (sizeof(int) + sizeof(int) + sizeof(int) + sizeof(int) + g_memorychunk_size[a_id]));

#if __h_config_memory_pool_tracking
  size += (CHUNK_ITEMS * (sizeof(int) + sizeof(int)));
#endif

  return size;
}


struct memorychunk *__alloc_new_chunk(struct memorypool *a_pool, int a_id)
{
  int size;
  struct memorychunk *chunk;

  size = __alloc_chunk_size(a_id);
  assert(size);

  chunk = (struct memorychunk *) OS_ALLOC(size);
  assert(chunk);

  return chunk;
}


void __free_chunk(struct memorychunk *a_chunk)
{
  assert(a_chunk);
  
  if (a_chunk->m_next)
  {
    __free_chunk(a_chunk->m_next);
  }

  OS_FREE(a_chunk);
}


void __free_heap(struct memorypool *a_pool, struct heapchunk *a_chunk)
{
  void *ptr;

  if (a_pool->m_heap == a_chunk)
  {
    a_pool->m_heap = a_chunk->m_next;
  }
  if (a_chunk->m_next)
  {
    a_chunk->m_next->m_prev = a_chunk->m_prev;
  }
  if (a_chunk->m_prev)
  {
    a_chunk->m_prev->m_next = a_chunk->m_next;
  }

  ptr = (void *) a_chunk;
  OS_FREE(ptr);
}


void *__chunk_item_data(struct memorychunk *a_chunk, int a_index)
{
  char *ptr;

  assert(a_chunk);
  assert(a_chunk->m_size);
  assert(a_index >= 0);

  ptr = (char *) a_chunk;
  ptr += sizeof(struct memorychunk);
  ptr += sizeof(int);
  ptr += sizeof(int);

#if __h_config_memory_pool_tracking
  ptr += sizeof(int);
  ptr += sizeof(int);
#endif

  ptr += sizeof(struct memorychunk *);
  ptr += (a_index * (sizeof(int) + sizeof(struct memorychunk) + a_chunk->m_size));

#if __h_config_memory_pool_tracking
  ptr += ((sizeof(int) + sizeof(int)) * a_index);
#endif

  return (void *) ptr;
}


int *__chunk_item_header(struct memorychunk *a_chunk, int a_index)
{
  char *ptr;

  assert(a_chunk);
  assert(a_chunk->m_size);
  assert(a_index >= 0);

  ptr = (char *) a_chunk;
  ptr += sizeof(struct memorychunk);
  ptr += (a_index * (sizeof(int) + sizeof(struct memorychunk) + a_chunk->m_size));

#if __h_config_memory_pool_tracking
  ptr += ((sizeof(int) + sizeof(int)) * a_index);
#endif

  return (int *) ptr;
}


int *__chunk_item_footer(struct memorychunk *a_chunk, int a_index)
{
  char *ptr;

  assert(a_chunk);
  assert(a_chunk->m_size);
  assert(a_index >= 0);

  ptr = (char *) a_chunk;
  ptr += sizeof(struct memorychunk);
  ptr += sizeof(int);
  ptr += sizeof(int);

#if __h_config_memory_pool_tracking
  ptr += sizeof(int);
  ptr += sizeof(int);
#endif

  ptr += sizeof(struct memorychunk *);
  ptr += a_chunk->m_size;
  ptr += (a_index * (sizeof(int) + sizeof(struct memorychunk) + a_chunk->m_size));

#if __h_config_memory_pool_tracking
  ptr += ((sizeof(int) + sizeof(int)) * a_index);
#endif

  return (int *) ptr;
}


void __show_pool_allocations(struct memorypool *a_pool)
{
  int i;

  assert(a_pool);
  assert(a_pool->m_num_alloc);

  _RPT0(0, "\nMemory pool has memory leaks!\n");

  for (i = 0; i < chunk_count; i++)
  {
    if (a_pool->m_chunks[i])
    {
      __show_chunk_allocations(a_pool->m_chunks[i]);
    }
  }

  if (a_pool->m_heap)
  {
    __show_heap_allocations(a_pool->m_heap);
  }

  _RPT0(0, "\n");
}


void __show_chunk_allocations(struct memorychunk *a_chunk)
{
  int i, mask, line, *header;
  const char *file;

  assert(a_chunk);

  if (a_chunk->m_next)
  {
    __show_chunk_allocations(a_chunk->m_next);
  }

  if (a_chunk->m_mask)
  {
    for (i = 0; i < CHUNK_ITEMS; i++)
    {
      mask = 1 << i;
      if ((mask & a_chunk->m_mask) != 0)
      {
#if __h_config_memory_pool_tracking

        header = __chunk_item_header(a_chunk, i);

        file = (const char *) (int *) header[1];
        line = (int) header[2];

        assert(header[0] == ALLOC_HEADER);

        __show_allocation("pool", file, line, a_chunk->m_size);
#else
        __show_allocation("pool", "unknown", 0, a_chunk->m_size);
#endif
      }
    }
  }
}


void __show_heap_allocations(struct heapchunk *a_chunk)
{
  assert(a_chunk);

#if __h_config_memory_pool_tracking
    __show_allocation("heap", a_chunk->m_file, a_chunk->m_line, a_chunk->m_size);
#else
    __show_allocation("", 0, a_chunk->m_size);
#endif

  if (a_chunk->m_next)
  {
    __show_heap_allocations(a_chunk->m_next);
  }
}


void __show_allocation(const char *a_source, const char *a_file, int a_line, int a_size)
{
  _RPT4(0, "  %04d byte %s chunk: %s(%d)\n", a_size, a_source, a_file, a_line);
}


// -- EOF

