#ifndef _HSTB_TPL_H_
#define _HSTB_TPL_H_

#include "uset.h"

#define uhstb_tpl(t)				\
  typedef struct _uhsnd_##t{			\
    struct _uhsnd_##t* next;			\
    t                  k;			\
  } uhsnd_##t;					\
  typedef struct _uhstb_iterator_##t{		\
    uhsnd_##t* next_nd;				\
    int next_idx;				\
  } uhstb_iterator_##t;				\
  typedef struct _uhstb_##t{			\
    USETHEADER;					\
    int len;					\
    uhsnd_##t* ndar[1];				\
  } uhstb_##t


#define uhstb_key_ft_tpl(t)			\
  typedef t(*uhstb_##t##_key_ft)(t);
#define uhstb_comp_ft_tpl(t)			\
  typedef int(*uhstb_##t##_comp_ft)(t,t);

#define uhstb_new_decl_tpl(t)			\
  uhstb_##t* uhstb_##t##_new(int size)

#define uhstb_put_decl_tpl(t)				\
  int uhstb_##t##_put(uhstb_##t*          hstb,		\
		      unsigned int        hscd,		\
		      t                   ink,		\
		      t**                 outk,		\
		      uhstb_##t##_comp_ft comp)

#define uhstb_get_decl_tpl(t)				\
  int uhstb_##t##_get(uhstb_##t*          hstb,		\
		      unsigned int        hscd,		\
		      t                   ink,		\
		      t**                 outk,		\
		      uhstb_##t##_comp_ft comp)

#define uhstb_decl_tpl(t)			\
  uhstb_tpl(t);					\
  uhstb_key_ft_tpl(t);				\
  uhstb_comp_ft_tpl(t);				\
  uhstb_new_decl_tpl(t);			\
  uhstb_put_decl_tpl(t);			\
  uhstb_get_decl_tpl(t)

#endif
