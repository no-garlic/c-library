
#ifndef __h_config
#define __h_config

// specify if memory pool allocations should use the pools, or if they should
// use malloc and free.
#define __h_config_memorypool_enabled 1

// should the file and line of each allocation be tracked (uses more memory but usefull
// to find out who has not unallocated memory.
#define __h_config_memory_pool_tracking 1

// inline function definition
#ifndef inline
#define inline __forceinline
#endif

// types
typedef unsigned int uint32;

#endif // __h_config

// -- EOF

