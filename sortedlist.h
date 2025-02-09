
#ifndef __h_sortedlist
#define __h_sortedlist

#ifdef  __cplusplus
extern "C" {
#endif // __cplusplus

#include "config.h"

// forward declarations
typedef struct sortedlist;
typedef struct sortedlist_item;
typedef struct memorypool;

// allocate a new sorted list
// a_valuesize the size of the value attribute to allocate for each item
// a_pool the memory pool to allocate from
// returns a pointer to the new sorted list
#define sortedlist_alloc(a_valuesize, a_pool) _sortedlist_alloc(a_valuesize, a_pool, __FILE__, __LINE__)

// frees a sorted list and all items
// a_list the list to operate on
#define sortedlist_free(a_list) _sortedlist_free(a_list)

// gets the number of items in the list
// a_list the list to operate on
// returns the number of items
#define sortedlist_count(a_list) _sortedlist_count(a_list)

// finds the first item in the list matching the given key
// a_list the list to operate on
// a_key the key to match
// returns the first item in the list matching the key, or 0 if not items match
#define sortedlist_find_first(a_list, a_key) _sortedlist_find_first(a_list, a_key)

// finds the next item in the list matching the key
// a_list the list to operate on
// a_key the key to match
// returns the first item in the list matching the key, or 0 if not items match
#define sortedlist_find_next(a_item) _sortedlist_find_next(a_item)

// gets the first item in the list
// a_list the list to operate on
// returns the item, or 0 if the list is empty
#define sortedlist_first(a_list) _sortedlist_first(a_list)

// gets the last item in the list
// a_list the list to operate on
// returns the item, or 0 if the list is empty
#define sortedlist_last(a_list) _sortedlist_last(a_list)

// gets the next item in the list
// a_item the item to get the next from
// returns the item, or 0 if the item supplied is the last item in the list
#define sortedlist_next(a_item) _sortedlist_next(a_item)

// gets the previous item in the list
// a_item the item to get the previous from
// returns the item, or 0 if the item supplied is the first item in the list
#define sortedlist_prev(a_item) _sortedlist_prev(a_item)

// gets the key for a list item
// a_item the item to operate on
// returns the key
#define sortedlist_key(a_item) _sortedlist_key(a_item)

// gets the value for a list item
// a_item the item to operate on
// returns the value
#define sortedlist_value(a_item) _sortedlist_value(a_item)

// inserts an item into the list
// a_list the list to operate on
// a_key the key to insert
// a_value the value to copy to the key (if not 0)
// returns a pointer to new item
#define sortedlist_insert(a_list, a_key, a_value) _sortedlist_insert(a_list, a_key, a_value, __FILE__, __LINE__)

// removes an item from the list
// a_item the item to remove
#define sortedlist_remove(a_item) _sortedlist_remove(a_item)

// interface functions
struct sortedlist *_sortedlist_alloc(int a_valuesize, struct memorypool *a_pool, const char *a_file, int a_line);
void _sortedlist_free(struct sortedlist *a_list);
int _sortedlist_count(struct sortedlist *a_list);
struct sortedlist_item *_sortedlist_find_first(struct sortedlist *a_list, int a_key);
struct sortedlist_item *_sortedlist_find_next(struct sortedlist_item *a_item);
struct sortedlist_item *_sortedlist_first(struct sortedlist *a_list);
struct sortedlist_item *_sortedlist_last(struct sortedlist *a_list);
struct sortedlist_item *_sortedlist_next(struct sortedlist_item *a_item);
struct sortedlist_item *_sortedlist_prev(struct sortedlist_item *a_item);
int _sortedlist_key(struct sortedlist_item *a_item);
void *_sortedlist_value(struct sortedlist_item *a_item);
struct sortedlist_item *_sortedlist_insert(struct sortedlist *a_list, int a_key, void *a_value, const char *a_file, int a_line);
void _sortedlist_remove(struct sortedlist_item *a_item);

#ifdef  __cplusplus
}
#endif // __cplusplus

#endif // __h_sortedlist

// -- EOF

