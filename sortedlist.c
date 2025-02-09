
#include "sortedlist.h"

#include <assert.h>
#include <memory.h>
#include <stdlib.h>

#include "memorypool.h"


struct sortedlist_item
{
  int m_key;
  struct sortedlist_item *m_next;
  struct sortedlist_item *m_prev;
  struct sortedlist *m_list;
};


struct sortedlist
{
  struct sortedlist_item *m_head;
  struct sortedlist_item *m_tail;
  struct memorypool *m_pool;
  int m_valuesize;
  int m_count;
};


struct sortedlist *_sortedlist_alloc(int a_valuesize, struct memorypool *a_pool, const char *a_file, int a_line)
{
  struct sortedlist *new_list;

  new_list = (struct sortedlist *) _palloc(a_file, a_line, a_pool, sizeof(struct sortedlist));
  assert(new_list);

  memset(new_list, 0, sizeof(struct sortedlist));
  new_list->m_valuesize = a_valuesize;
  new_list->m_pool = a_pool;

  return new_list;
}


void _sortedlist_free(struct sortedlist *a_list)
{
  struct sortedlist_item *ptr, *tmp;

  ptr = a_list->m_head;

  while (ptr)
  {
    tmp = ptr;
    ptr = ptr->m_next;
    pfree(a_list->m_pool, tmp);
  }

  pfree(a_list->m_pool, a_list);
}


int _sortedlist_count(struct sortedlist *a_list)
{
  assert(a_list);
  return a_list->m_count;
}


struct sortedlist_item *_sortedlist_find_first(struct sortedlist *a_list, int a_key)
{
  struct sortedlist_item *ptr;

  ptr = a_list->m_head;

  while (ptr)
  {
    if (ptr->m_key == a_key)
    {
      return ptr;
    }

    ptr = ptr->m_next;
  }

  return 0;
}


struct sortedlist_item *_sortedlist_find_next(struct sortedlist_item *a_item)
{
  assert(a_item);

  if (a_item->m_next != 0 && a_item->m_next->m_key == a_item->m_key)
  {
    return a_item->m_next;
  }

  return 0;
}


struct sortedlist_item *_sortedlist_first(struct sortedlist *a_list)
{
  assert(a_list);
  return a_list->m_head;
}


struct sortedlist_item *_sortedlist_last(struct sortedlist *a_list)
{
  assert(a_list);
  return a_list->m_tail;
}


struct sortedlist_item *_sortedlist_next(struct sortedlist_item *a_item)
{
  assert(a_item);
  return a_item->m_next;
}


struct sortedlist_item *_sortedlist_prev(struct sortedlist_item *a_item)
{
  assert(a_item);
  return a_item->m_prev;
}


int _sortedlist_key(struct sortedlist_item *a_item)
{
  assert(a_item);
  return a_item->m_key;
}


void *_sortedlist_value(struct sortedlist_item *a_item)
{
  struct sortedlist *itemlist;
  char *value;

  assert(a_item);
  assert(a_item->m_list);

  itemlist = a_item->m_list;
  assert(itemlist);

  if (itemlist->m_valuesize)
  {
    value = (char *) a_item;
    value += sizeof(struct sortedlist_item);
    return (void *) value;
  }
  
  return 0;
}


struct sortedlist_item *_sortedlist_insert(struct sortedlist *a_list, int a_key, void *a_value, const char *a_file, int a_line)
{
  struct sortedlist_item *item, *ptr, *prev;
  void *value;
  char *memory;
  int allocsize;

  assert(a_list);

  allocsize = sizeof(struct sortedlist_item) + a_list->m_valuesize;

  memory = (char *) _palloc(a_file, a_line, a_list->m_pool, allocsize);
  assert(memory);

  memset(memory , 0, allocsize);

  item = (struct sortedlist_item *) memory;
  assert(item);

  a_list->m_count++;

  item->m_list = a_list;
  item->m_key = a_key;
  item->m_next = 0;
  item->m_prev = 0;

  if (a_list->m_valuesize)
  {
    memory += sizeof(struct sortedlist_item);
    value = (void *) memory;

    if (a_value)
    {
      memcpy(value, a_value, a_list->m_valuesize);
    }
    else
    {
      memset(value, 0, a_list->m_valuesize);
    }
  }

  ptr = a_list->m_head;
  
  if (!ptr)
  {
    a_list->m_head = item;
    a_list->m_tail = item;
    return item;
  }

  if (a_key <= ptr->m_key)
  {
    item->m_next = ptr;
    a_list->m_head = item;
    ptr->m_prev = item;
    return item;
  }

  prev = ptr;
  ptr = ptr->m_next;

  while (ptr)
  {
    if (a_key <= ptr->m_key)
    {
      item->m_next = ptr;
      item->m_prev = prev;
      prev->m_next = item;
      ptr->m_prev = item;
      return item;
    }

    prev = ptr;
    ptr = ptr->m_next;
  }

  item->m_prev = a_list->m_tail;
  a_list->m_tail->m_next = item;
  a_list->m_tail = item;
  return item;
}


void _sortedlist_remove(struct sortedlist_item *a_item)
{
  assert(a_item);
  assert(a_item->m_list);
  assert(a_item->m_list->m_count > 0);

  if (a_item->m_list->m_head == a_item)
  {
    a_item->m_list->m_head = a_item->m_list->m_head->m_next;
    if (a_item->m_list->m_head)
    {
      a_item->m_list->m_head->m_prev = 0;
    }
  }
  else if (a_item->m_list->m_tail == a_item)
  {
    a_item->m_list->m_tail = a_item->m_list->m_tail->m_prev;
    if (a_item->m_list->m_tail)
    {
      a_item->m_list->m_tail->m_next = 0;
    }
  }
  else
  {
    assert(a_item->m_prev);
    assert(a_item->m_next);

    a_item->m_next->m_prev = a_item->m_prev;
    a_item->m_prev->m_next = a_item->m_next;
  }

  a_item->m_list->m_count--;

  pfree(a_item->m_list->m_pool, a_item);
}


// -- EOF

