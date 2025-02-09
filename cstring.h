
#ifndef __h_cstring
#define __h_cstring

#ifdef  __cplusplus
extern "C" {
#endif // __cplusplus

#include "config.h"

// forward declarations
typedef struct cstring;
typedef struct mempool;

// hash a string into an int
// a_str the string to hash
// returns an int value thats is highly likely to be unique
inline int string_hash(const char *a_str)
{
  // locals
  int hash;
  char chr;

  // iterate over each chracter in the string
  for (hash = 0; *a_str; a_str++) 
  {
    chr = *a_str;
    chr = (char)(chr - (chr & (chr >> 1) & 0x20));
    hash = 131 * hash + chr;
  }

  // return the hash
  return hash;
}

// allocate a new cstring
// a_str the string to initialise the new cstring with
// a_pool the memory pool to allocate from
// returns the newly allocated cstring
#define cstring_alloc(a_str, a_pool) _cstring_alloc(a_str, a_pool, __FILE__, __LINE__)

// free the cstring and all allocated memory
// a_cstr the cstring to operate on
#define cstring_free(a_cstr) _cstring_free(a_cstr)

// extract a const char * value of a cstring
// a_cstr the cstring to operate on
// returns a const char pointer to the start of the string
#define cstring_cstr(a_cstr) _cstring_cstr(a_cstr)

// hash a cstring into an int
// a_cstr the cstring to operate on
// returns an int value thats is highly likely to be unique
#define cstring_hash(a_cstr) _cstring_hash(a_cstr)

// get the length of the cstring
// a_cstr the cstring to operate on
// returns the length of the cstring (not including the null terminator)
#define cstring_len(a_cstr) _cstring_len(a_cstr)

// reserve space in the string
// a_cstr the cstring to operate on
// a_count the number of characters that the string should reserve memory for
#define cstring_reserve(a_string, a_count) _cstring_reserve(a_string, a_count, __FILE__, __LINE__)

// set the value of a cstring
// a_cstr the cstring to operate on
// a_str the const char value to set
#define cstring_set(a_cstr, a_str) _cstring_set(a_cstr, a_str, __FILE__, __LINE__)

// set the value of a cstring using variable args
// a_cstr the cstring to operate on
// a_format, ... the string and arguments
#define cstring_setf(a_cstr, a_format, ...) _cstring_setf(a_cstr, __FILE__, __LINE__, a_format, __VA_ARGS__)

// append a string to a cstring
// a_cstr the cstring to operate on
// a_str the string to append
#define cstring_cat(a_cstr, a_str) _cstring_cat(a_cstr, a_str, __FILE__, __LINE__)

// append a string to a cstring using variable args
// a_cstr the cstring to operate on
// a_format, ... the string and arguments
#define cstring_catf(a_cstr, a_format, ...) _cstring_catf(a_cstr, __FILE__, __LINE__, a_format, __VA_ARGS__)

// delete characters from a cstring
// a_cstr the cstring to operate on
// a_start the first character to delete
// a_count the number of characters to delete
#define cstring_del(a_cstr, a_start, a_count) _cstring_del(a_cstr, a_start, a_count)

// insert characters into a cstring
// a_cstr the cstring to operate on
// a_start the index of where the characters should be inserted
// a_count the number of characters to insert
// a_str the string to copy to the newly inserted characters
#define cstring_ins(a_cstr, a_start, a_count, a_str) _cstring_ins(a_cstr, a_start, a_count, a_str, __FILE__, __LINE__)

// interface functions
struct cstring *_cstring_alloc(const char *a_str, struct memorypool *a_pool, const char *a_file, int a_line);
void _cstring_free(struct cstring *a_cstr);
const char *_cstring_cstr(struct cstring *a_cstr);
int _cstring_hash(struct cstring *a_cstr);
int _cstring_len(struct cstring *a_cstr);
void _cstring_reserve(struct cstring *a_string, int a_count, const char *a_file, int a_line);
void _cstring_set(struct cstring *a_cstr, const char *a_str, const char *a_file, int a_line);
void _cstring_setf(struct cstring *a_cstr, const char *a_file, int a_line, const char *a_format, ...);
void _cstring_cat(struct cstring *a_cstr, const char *a_str, const char *a_file, int a_line);
void _cstring_catf(struct cstring *a_cstr, const char *a_file, int a_line, const char *a_format, ...);
void _cstring_del(struct cstring *a_cstr, int a_start, int a_count);
void _cstring_ins(struct cstring *a_cstr, int a_start, int a_count, const char *a_str, const char *a_file, int a_line);

#ifdef  __cplusplus
}
#endif // __cplusplus

#endif // __h_cstring

// -- EOF

