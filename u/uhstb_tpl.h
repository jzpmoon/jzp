#ifndef _UHSTB_TPL_H_
#define _UHSTB_TPL_H_

#include "umacro.h"
#include "uset.h"
#include "ualloc.h"

#define UHSTB_LENGTH 31

#define uhstb_tpl(t)				\
  typedef struct _uhsnd_##t{			\
    struct _uhsnd_##t* next;			\
    t                  k;			\
  } uhsnd_##t;					\
  typedef struct _uhstb_##t##_cursor{		\
    uhsnd_##t* next_nd;				\
    int next_idx;				\
  } uhstb_##t##_cursor;				\
  typedef struct _uhstb_##t{			\
    USETHEADER;					\
    int len;					\
    int count;					\
    uallocator* allocator;			\
    uhsnd_##t* ndar[1];				\
  } uhstb_##t;					\
  typedef int(ucall *uhstb_##t##_dest_ft)(t)

#define uhstb_key_ft_tpl(t)			\
  typedef t(ucall *uhstb_##t##_key_ft)(t*)

#define uhstb_comp_ft_tpl(t)			\
  typedef int(ucall *uhstb_##t##_comp_ft)(t*,t*)

#define uhstb_new_decl_tpl(t)			\
  uapi_tpl uhstb_##t* ucall uhstb_##t##_new(int len)

#define uhstb_alloc_decl_tpl(t)					\
  uapi_tpl uhstb_##t* ucall uhstb_##t##_alloc(uallocator* allocator,int len)

#define uhstb_dest_decl_tpl(t)						\
  uapi_tpl void ucall uhstb_##t##_dest(uhstb_##t* hstb,uhstb_##t##_dest_ft dest)

#define uhstb_put_decl_tpl(t)				\
  uapi_tpl int ucall uhstb_##t##_put(uhstb_##t*          hstb,		\
		      unsigned int        hscd,		\
		      t*                  ink,		\
		      t**                 outk,		\
		      uhstb_##t##_key_ft  putk,		\
		      uhstb_##t##_comp_ft comp)

#define uhstb_get_decl_tpl(t)				\
  uapi_tpl int ucall uhstb_##t##_get(uhstb_##t*          hstb,		\
		      unsigned int        hscd,		\
		      t*                  ink,		\
		      t**                 outk,		\
		      uhstb_##t##_comp_ft comp)

#define uhstb_decl_tpl(t)			\
  uhstb_tpl(t);					\
  uhstb_key_ft_tpl(t);				\
  uhstb_comp_ft_tpl(t);				\
  uhstb_new_decl_tpl(t);			\
  uhstb_alloc_decl_tpl(t);			\
  uhstb_dest_decl_tpl(t);			\
  uhstb_put_decl_tpl(t);			\
  uhstb_get_decl_tpl(t)

#endif
