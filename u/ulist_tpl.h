#ifndef _ULIST_TPL_H_
#define _ULIST_TPL_H_

#include "udef.h"
#include "uset.h"
#include "umempool.h"

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
    uallocator* allocator;			\
    int len;					\
    ulsnd_##t* header;				\
  } ulist_##t;					\
  typedef void(*ulist_##t##_dest_ft)(t)

#define ulist_new_decl_tpl(t)			\
  ulist_##t* ulist_##t##_new()

#define ulist_alloc_decl_tpl(t)				\
  ulist_##t* ulist_##t##_alloc(uallocator* allocator)
  
#define ulist_append_decl_tpl(t)			\
  int ulist_##t##_append(ulist_##t* list,t value)

#define ulist_concat_decl_tpl(t)				\
  void ulist_##t##_concat(ulist_##t* list1,ulist_##t* list2)

#define ulist_dest_decl_tpl(t)					\
  void ulist_##t##_dest(ulist_##t* list,ulist_##t##_dest_ft del)
  
#define ulist_decl_tpl(t)			\
  ulist_tpl(t);					\
  ulist_new_decl_tpl(t);			\
  ulist_alloc_decl_tpl(t);			\
  ulist_append_decl_tpl(t);			\
  ulist_concat_decl_tpl(t);			\
  ulist_dest_decl_tpl(t)
  
#endif
