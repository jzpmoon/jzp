#ifndef _ULIST_TPL_H_
#define _ULIST_TPL_H_

#include "udef.h"
#include "uset.h"

#define ulist_tpl(t)				\
  typedef struct _ulsnd_##t{			\
    struct _ulsnd_##t* prev;			\
    struct _ulsnd_##t* next;			\
    t value;					\
  } ulsnd_##t;					\
  typedef struct _ulist_##t##_cursor{		\
    ulsnd_##t* next;				\
  } ulist_##t##_cursor;				\
  typedef struct _ulist_##t{			\
    USETHEADER;					\
    int len;					\
    ulsnd_##t* header;				\
  } ulist_##t

#define ulist_new_decl_tpl(t)			\
  ulist_##t* ulist_##t##_new()
  
#define ulist_append_decl_tpl(t)			\
  int ulist_##t##_append(ulist_##t* list,t value)

#define ulist_decl_tpl(t)			\
  ulist_tpl(t);					\
  ulist_new_decl_tpl(t);			\
  ulist_append_decl_tpl(t)
  
#endif
