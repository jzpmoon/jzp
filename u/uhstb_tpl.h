#ifndef _UHSTB_TPL_H_
#define _UHSTB_TPL_H_

#include "uset.h"
#include "umempool.h"

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
    umem_pool* pool;				\
    uhsnd_##t* ndar[1];				\
  } uhstb_##t

#define uhstb_key_ft_tpl(t)			\
  typedef t(*uhstb_##t##_key_ft)(t*)

#define uhstb_comp_ft_tpl(t)			\
  typedef int(*uhstb_##t##_comp_ft)(t*,t*)

#define uhstb_new_decl_tpl(t)			\
  uhstb_##t* uhstb_##t##_new(int len)

#define uhstb_newmp_decl_tpl(t)				\
  uhstb_##t* uhstb_##t##_newmp(umem_pool*pool,int len)

#define uhstb_put_decl_tpl(t)				\
  int uhstb_##t##_put(uhstb_##t*          hstb,		\
		      unsigned int        hscd,		\
		      t*                  ink,		\
		      t**                 outk,		\
		      uhstb_##t##_key_ft  putk,		\
		      uhstb_##t##_comp_ft comp)

#define uhstb_get_decl_tpl(t)				\
  int uhstb_##t##_get(uhstb_##t*          hstb,		\
		      unsigned int        hscd,		\
		      t*                  ink,		\
		      t**                 outk,		\
		      uhstb_##t##_comp_ft comp)

#define uhstb_decl_tpl(t)			\
  uhstb_tpl(t);					\
  uhstb_key_ft_tpl(t);				\
  uhstb_comp_ft_tpl(t);				\
  uhstb_new_decl_tpl(t);			\
  uhstb_newmp_decl_tpl(t);			\
  uhstb_put_decl_tpl(t);			\
  uhstb_get_decl_tpl(t)

#endif
