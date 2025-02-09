
#include "config.h"

#include <assert.h>
#include <stdio.h>
#include <conio.h>
#include <memory.h>
#include <crtdbg.h>

#include "array.h"
#include "cstring.h"
#include "vector.h"
#include "tree.h"
#include "sortedlist.h"
#include "memorypool.h"


void test_memorypool();
void test_array();
void test_vector();
void test_string();
void test_tree();
void test_sortedlist();
void print_sortedlist(struct sortedlist *a_list);


static int a = 10;
static int b = 20;
static int c = 30;
static int d = 40;
static int e = 50;
static int f = 60;


int main(int a_argc, char *a_argv[])
{
  struct memorypool *globalpool;
  globalpool = memorypool_alloc();
  memorypool_global(globalpool);

  test_memorypool();
  test_array();
  test_vector();
  test_string();
  test_sortedlist();
  test_tree();

  memorypool_free(globalpool);
  globalpool = 0;

  getch();
  return 0;
}


tree_enum_result test_tree_func(struct treenode *a_node)
{
  int key, val;

  key = treenode_key(a_node);
  val = (int) * (int*) treenode_value(a_node);

  printf("Node: %d = %d\n", key, val);
  return tree_enum_ok;
}


void test_tree()
{
  struct tree *items;
  items = tree_alloc(sizeof(int), 0);

  tree_insert(items, 2, &b);
  tree_insert(items, 4, &d);
  tree_insert(items, 1, &a);
  tree_insert(items, 3, &c);
  tree_insert(items, 5, &e);
  tree_insert(items, 6, &f);

  tree_enum(items, test_tree_func);
  printf("--------------------------------\n\n");

  tree_find_remove(items, 5);
  tree_find_remove(items, 6);
  tree_find_remove(items, 4);
  tree_insert(items, 6, &f);
  tree_insert(items, 5, &e);
  tree_insert(items, 4, &d);

  tree_enum(items, test_tree_func);
  printf("--------------------------------\n\n");

  tree_find_remove_subtree(items, 4);
  tree_find_remove(items, 2);
  tree_insert(items, 2, &b);

  tree_enum(items, test_tree_func);
  printf("--------------------------------\n\n");

  tree_free(items);
}


void test_array()
{
  struct array *items;
  unsigned char *data;
  int count;
  int elemsize;

  items = array_alloc(sizeof(int), 8, 0);
  data = (unsigned char *) array_data(items);
  
  array_zero(items);

  count = array_count(items);
  elemsize = array_elemsize(items);
  printf("Array: Count(%d), Size(%d)\n", count, elemsize);

  array_resize(items, 10);
  data = (unsigned char *) array_data(items);

  count = array_count(items);
  elemsize = array_elemsize(items);
  printf("Array: Count(%d), Size(%d)\n", count, elemsize);

  array_free(items);
  items = 0;
}


void test_vector()
{
  struct vector *items;
  int *data;
  int count;
  int capacity;
  int elemsize;
  
  items = vector_alloc(sizeof(int), 0);

  vector_resize(items, 2);
  vector_zero(items);

  count = vector_count(items);
  capacity = vector_capacity(items);
  elemsize = vector_elemsize(items);
  printf("Vector: Count(%d), Capacity(%d), Size(%d)\n", count, capacity, elemsize);

  data = vector_append(items, 3);
  *data++ = 1;
  *data++ = 2;
  *data = 3;

  vector_remove(items, 1, 2);

  data = vector_insert(items, 1, 2);
  *data++ = 0;
  *data = 1;

  vector_free(items);
  items = 0;
}


void test_sortedlist()
{
  struct sortedlist *items;
  struct sortedlist_item *item;
  int key;
  int value;

  items = sortedlist_alloc(sizeof(int), 0);

  key = 3;
  value = 30;
  item = sortedlist_insert(items, key, &value);

  print_sortedlist(items);

  key = 1;
  value = 10;
  item = sortedlist_insert(items, key, &value);

  print_sortedlist(items);

  key = 2;
  value = 20;
  item = sortedlist_insert(items, key, &value);

  print_sortedlist(items);

  key = 3;
  value = 31;
  item = sortedlist_insert(items, key, &value);

  print_sortedlist(items);

  key = 5;
  value = 50;
  item = sortedlist_insert(items, key, &value);

  print_sortedlist(items);

  key = 3;
  value = 32;
  item = sortedlist_insert(items, key, &value);

  print_sortedlist(items);

  key = 4;
  value = 40;
  item = sortedlist_insert(items, key, &value);

  print_sortedlist(items);

  item = sortedlist_find_first(items, 3);
  item = sortedlist_find_next(item);
  sortedlist_remove(item);

  print_sortedlist(items);

  sortedlist_free(items);
  items = 0;
}


void print_sortedlist(struct sortedlist *a_list)
{
  struct sortedlist_item *item, *itemprev, *itemnext;
  int key;
  int value;
  int i;

  i = 0;
  item = sortedlist_first(a_list);
  value = (int) (int*) sortedlist_value(item);

  while (item)
  {
    key = sortedlist_key(item);
    value = * (int*) sortedlist_value(item);
    printf("List(%d): %d = %d", i, key, value);

    itemprev = sortedlist_prev(item);
    if (itemprev)
    {
      key = sortedlist_key(itemprev);
      value = * (int*) sortedlist_value(itemprev);
      printf(" \tPrev: %d = %d", key, value);
    }
    else
    {
      printf(" \tPrev: NULL ");
    }
    
    itemnext = sortedlist_next(item);
    if (itemnext)
    {
      key = sortedlist_key(itemnext);
      value = * (int*) sortedlist_value(itemnext);
      printf(" \tNext: %d = %d", key, value);
    }
    else
    {
      printf(" \tNext: NULL ");
    }

    printf("\n");

    ++i;
    item = sortedlist_next(item);
  }

  printf("----------------------------------------------------\n\n");
}


void test_string()
{
  struct cstring *cstr;

  cstr = cstring_alloc("Test", 0);
  printf("String: [%s] - Len: %d\n", cstring_cstr(cstr), cstring_len(cstr));

  cstring_cat(cstr, " 100");
  printf("String: [%s] - Len: %d\n", cstring_cstr(cstr), cstring_len(cstr));

  cstring_catf(cstr, " %d", 200);
  printf("String: [%s] - Len: %d\n", cstring_cstr(cstr), cstring_len(cstr));

  cstring_del(cstr, 1, 2);
  printf("String: [%s] - Len: %d\n", cstring_cstr(cstr), cstring_len(cstr));

  cstring_del(cstr, 1, 2);
  printf("String: [%s] - Len: %d\n", cstring_cstr(cstr), cstring_len(cstr));

  cstring_ins(cstr, 1, 4, "est ");
  printf("String: [%s] - Len: %d\n", cstring_cstr(cstr), cstring_len(cstr));

  cstring_setf(cstr, "Test [%s] : [%d]", "String", 100);
  printf("String: [%s] - Len: %d\n", cstring_cstr(cstr), cstring_len(cstr));

  cstring_free(cstr);
  cstr = 0;
}


void test_memorypool()
{
  struct memorypool *pool;
  int i;
  int *memarray[256];
  
  pool = memorypool_alloc();
  assert(pool);

  for (i = 0; i < 256; i++)
  {
    memarray[i] = palloc(pool, i*4);
  }

  for (i = 0; i < 256; i++)
  {
    pfree(pool, memarray[i]);
  }

  memorypool_trunc(pool);

  for (i = 1; i < 16; i++)
  {
    memarray[i] = palloc(pool, i*4);
  }

  for (i = 1; i < 16; i++)
  {
    prealloc(pool, memarray[i], i*32);
  }

  for (i = 1; i < 16; i++)
  {
    prealloc(pool, memarray[i], i*2);
  }

  for (i = 1; i < 16; i++)
  {
    pfree(pool, memarray[i]);
  }

  memorypool_free(pool);

  pool = 0;
}



