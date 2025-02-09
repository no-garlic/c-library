# C Library for Containers and String Utilities

## Overview
This C library provides a set of container data structures and string manipulation utilities designed for high performance and ease of use. The library includes implementations of dynamic arrays, sorted lists, binary trees, vectors, memory pools, and string utilities, along with console helpers. Each module is designed with efficiency in mind, providing safe memory management and easy-to-use APIs for C developers.

## Features
- **Containers**: Dynamic arrays, sorted lists, binary trees, and vectors
- **String Functions**: Flexible string manipulation utilities
- **Memory Management**: Efficient memory pool allocation
- **Console Utilities**: Helper functions for formatted console output
- **Configurable**: Modular and easy-to-integrate into C projects

## Installation
To use this library in your project, include the relevant headers and source files in your build system. 
## Usage Guide

### 1. Dynamic Arrays (`array.h`)

**API:**
```c
typedef struct {
    void *data;
    size_t element_size;
    size_t capacity;
    size_t size;
} array_t;

array_t array_create(size_t element_size, size_t capacity);
void array_push(array_t *array, void *element);
void array_pop(array_t *array, void *out_element);
void array_destroy(array_t *array);
```

**Example:**
```c
#include "array.h"
int main() {
    array_t my_array = array_create(sizeof(int), 10);
    int value = 42;
    array_push(&my_array, &value);
    array_destroy(&my_array);
    return 0;
}
```

### 2. Sorted List (`sortedlist.h`)

**API:**
```c
typedef struct sortedlist_t sortedlist_t;

void sortedlist_init(sortedlist_t *list, size_t element_size, int (*compare)(const void *, const void *));
void sortedlist_insert(sortedlist_t *list, void *element);
void sortedlist_remove(sortedlist_t *list, void *element);
void *sortedlist_find(sortedlist_t *list, void *element);
void sortedlist_destroy(sortedlist_t *list);
```

**Example:**
```c
#include "sortedlist.h"
int main() {
    sortedlist_t list;
    sortedlist_init(&list, sizeof(int), compare_ints);
    int value = 100;
    sortedlist_insert(&list, &value);
    sortedlist_destroy(&list);
    return 0;
}
```

### 3. Binary Trees (`tree.h`)

**API:**
```c
typedef struct tree_t tree_t;

tree_t *tree_create(int (*compare)(const void *, const void *));
void tree_insert(tree_t *tree, void *data);
void tree_remove(tree_t *tree, void *data);
void *tree_find(tree_t *tree, void *data);
void tree_destroy(tree_t *tree);
```

**Example:**
```c
#include "tree.h"
int main() {
    tree_t *tree = tree_create(compare_nodes);
    int key = 5;
    tree_insert(tree, &key);
    tree_destroy(tree);
    return 0;
}
```

### 4. Vector (`vector.h`)

**API:**
```c
typedef struct vector_t vector_t;

vector_t *vector_create(size_t element_size);
void vector_push(vector_t *vector, void *element);
void *vector_get(vector_t *vector, size_t index);
void vector_destroy(vector_t *vector);
```

### 5. Memory Pool (`memorypool.h`)

**API:**
```c
typedef struct memorypool_t memorypool_t;

void memorypool_init(memorypool_t *pool, size_t block_size, size_t block_count);
void *memorypool_alloc(memorypool_t *pool);
void memorypool_free(memorypool_t *pool, void *ptr);
void memorypool_destroy(memorypool_t *pool);
```

### 6. String Utilities (`cstring.h`)

**API:**
```c
size_t cstring_length(const char *str);
void cstring_copy(char *dest, const char *src);
void cstring_concat(char *dest, const char *src, size_t max_size);
```

### 7. Console Utilities (`console.h`)

**API:**
```c
void console_printf(const char *format, ...);
void console_readline(char *buffer, size_t size);
```

## License
This library is licensed under the MIT License. See `LICENSE` for details.

## Contribution
Contributions are welcome! Please submit issues and pull requests via GitHub.

