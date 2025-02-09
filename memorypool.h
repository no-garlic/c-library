
#ifndef __h_memorypool
#define __h_memorypool

#ifdef  __cplusplus
extern "C" {
#endif // __cplusplus

#include "config.h"

// forward declarations
typedef struct memorypool;

// allocte a new memory pool
// returns a pointer to the memory pool
#define memorypool_alloc() _memorypool_alloc()

// frees a memory pool (if there are no outstanding allocations, otherwise no action is taken)
// a_pool the memory pool to operate on
// returns 0 if no allocations were open and the pool was freed
#define memorypool_free(a_pool) _memorypool_free(a_pool)

// truncates a memory pool (which forcefully frees all memory and sets the allocation count to 0)
// a_pool the memory pool to operate on
#define memorypool_trunc(a_pool) _memorypool_trunc(a_pool)

// set the global memory pool, which is used if 0 is specified as a memory pool or the gp* macros are used
// a_pool the memory pool to use
#define memorypool_global(a_pool) _memorypool_global(a_pool)

// reports all allocations that are outstanding in the memory pool
// a_pool the memory pool to report on
#define memorypool_report(a_pool) _memorypool_report(a_pool)

// allocate memory from the given memory pool
// a_pool the pool to allocate from, otherwise the global pool is used if 0 is specified
// returns a pointer to the allocated memory or 0 on failure
#define palloc(a_pool, a_size) _palloc(__FILE__, __LINE__, a_pool, a_size)

// reallocate memory from the given memory pool
// a_pool the pool to reallocate from, otherwise the global pool is used if 0 is specified
// returns a pointer to the new allocated memory or 0 on failure
#define prealloc(a_pool, a_mem, a_size) _prealloc(__FILE__, __LINE__, a_pool, a_mem, a_size)

// free the memory from the given memory pool
// a_pool the pool to free the memory from
// a_mem a pointer to the memory to free
#define pfree(a_pool, a_mem) _pfree(a_pool, a_mem)

// allocate memory from the global memory pool
// returns a pointer to the allocated memory or 0 on failure
#define gpalloc(a_size) _palloc(__FILE__, __LINE__, 0, a_size)

// reallocate memory from the global memory pool
// returns a pointer to the new allocated memory or 0 on failure
#define gprealloc(a_mem, a_size) _prealloc(__FILE__, __LINE__, 0, a_mem, a_size)

// free the memory from the global memory pool
// a_mem a pointer to the memory to free
#define gpfree(a_mem) _pfree(0, a_mem)

// interface functions
struct memorypool *_memorypool_alloc();
int _memorypool_free(struct memorypool *a_pool);
void _memorypool_trunc(struct memorypool *a_pool);
void _memorypool_global(struct memorypool *a_pool);
void _memorypool_report(struct memorypool *a_pool);
void *_palloc(const char *a_file, int a_line, struct memorypool *a_pool, uint32 a_size);
void *_prealloc(const char *a_file, int a_line, struct memorypool *a_pool, void *a_mem, uint32 a_size);
void _pfree(struct memorypool *a_pool, void *a_mem);

#ifdef  __cplusplus
}
#endif // __cplusplus

#endif // __h_memorypool

// -- EOF

