#ifndef _USET_H_
#define _USET_H_

#include "umacro.h"

struct _uset;

typedef struct _ucursor{
  char pad[32];  
} ucursor;

typedef void (ucall * uset_cursor_ft)(ucursor* cursor);
typedef void*(ucall * uset_next_ft)(struct _uset* set,ucursor* cursor);

#define USETHEADER				\
  uset_cursor_ft iterate;			\
  uset_next_ft next

typedef struct _uset{
  USETHEADER;
} uset;

typedef void** unext;
#define unext_get(next) *(next);

#endif
