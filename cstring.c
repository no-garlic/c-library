
#include "cstring.h"

#include <assert.h>
#include <memory.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "memorypool.h"
#include "vector.h"


struct cstring
{
  struct vector *m_chars;
  struct memorypool *m_pool;
};


struct cstring *_cstring_alloc(const char *a_str, struct memorypool *a_pool, const char *a_file, int a_line)
{
  struct cstring *c_str;
  char *data;
  int length;

  c_str = (struct cstring *) _palloc(a_file, a_line, a_pool, sizeof(struct cstring));
  assert(c_str);

  c_str->m_pool = a_pool;

  length = a_str ? strlen(a_str) : 0;
  c_str->m_chars = _vector_alloc(sizeof(char), 0, a_file, a_line);
  assert(c_str->m_chars);

  _vector_resize(c_str->m_chars, length + 1, a_file, a_line);
  data = (char *) vector_data(c_str->m_chars);
  assert(data);

  if (a_str)
  {
    strcpy(data, a_str);
  }
  else
  {
    vector_zero(c_str->m_chars);
  }

  return c_str;
}


void _cstring_free(struct cstring *a_cstr)
{
  assert(a_cstr);

  vector_free(a_cstr->m_chars);
  pfree(a_cstr->m_pool, a_cstr);
}


const char *_cstring_cstr(struct cstring *a_cstr)
{
  char *data;

  assert(a_cstr);

  data = (char *) vector_data(a_cstr->m_chars);
  assert(data);

  return data;
}


int _cstring_hash(struct cstring *a_cstr)
{
  const char *str;

  assert(a_cstr);

  str = (const char *) vector_data(a_cstr->m_chars);
  return string_hash(str);
}


int _cstring_len(struct cstring *a_cstr)
{
  assert(a_cstr);
  return vector_count(a_cstr->m_chars) - 1;
}


void _cstring_reserve(struct cstring *a_cstr, int a_count, const char *a_file, int a_line)
{
  assert(a_cstr);
  assert(a_count > cstring_len(a_cstr));

  _vector_reserve(a_cstr->m_chars, a_count, a_file, a_line);
}


void _cstring_set(struct cstring *a_cstr, const char *a_str, const char *a_file, int a_line)
{
  char *data;
  int length;

  assert(a_cstr);
  assert(a_str);

  length = a_str ? strlen(a_str) : 0;
  _vector_resize(a_cstr->m_chars, length + 1, a_file, a_line);

  data = (char *) vector_data(a_cstr->m_chars);
  assert(data);

  if (a_str)
  {
    strcpy(data, a_str);
  }
  else
  {
    vector_zero(a_cstr->m_chars);
  }
}


void _cstring_setf(struct cstring *a_cstr, const char *a_file, int a_line, const char *a_format, ...)
{
  char *data;
  int length;
  va_list args;

  assert(a_cstr);
  assert(a_format);

  va_start(args, a_format);

  length = _vscprintf(a_format, args);
  _vector_resize(a_cstr->m_chars, length + 1, a_file, a_line);

  data = (char *) vector_data(a_cstr->m_chars);
  assert(data);

  vsprintf(data, a_format, args);
  va_end(args);
}


void _cstring_cat(struct cstring *a_cstr, const char *a_str, const char *a_file, int a_line)
{
  char *data;
  int curlen, newlen;

  assert(a_cstr);
  assert(a_str);

  curlen = vector_count(a_cstr->m_chars);
  assert(curlen);

  newlen = strlen(a_str);
  assert(newlen);

  _vector_resize(a_cstr->m_chars, curlen + newlen, a_file, a_line);
  data = (char *) vector_data(a_cstr->m_chars);
  assert(data);
  data += (curlen - 1);

  strcpy(data, a_str);
}


void _cstring_catf(struct cstring *a_cstr, const char *a_file, int a_line, const char *a_format, ...)
{
  char *data;
  int curlen, newlen;
  va_list args;

  assert(a_cstr);
  assert(a_format);

  va_start(args, a_format);

  curlen = vector_count(a_cstr->m_chars);
  assert(curlen);

  newlen = _vscprintf(a_format, args);
  assert(newlen);

  _vector_resize(a_cstr->m_chars, curlen + newlen, a_file, a_line);
  data = (char *) vector_data(a_cstr->m_chars);
  assert(data);
  data += (curlen - 1);

  vsprintf(data, a_format, args);
  va_end(args);
}


void _cstring_del(struct cstring *a_cstr, int a_start, int a_count)
{
  assert(a_cstr);
  assert(a_count);

  vector_remove(a_cstr->m_chars, a_start, a_count);
}


void _cstring_ins(struct cstring *a_cstr, int a_start, int a_count, const char *a_str, const char *a_file, int a_line)
{
  char *data;

  assert(a_cstr);
  assert(a_count);

  data = _vector_insert(a_cstr->m_chars, a_start, a_count, a_file, a_line);
  memcpy(data, a_str, a_count * sizeof(char));
}


// -- EOF

