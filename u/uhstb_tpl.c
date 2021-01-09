#include <stddef.h>
#include <string.h>
#include "ualloc.h"
#include "uerror.h"
#include "uhstb_tpl.h"

#define uhstb_cursor_tpl(t)					\
  static void uhstb_##t##_cursor_init(ucursor* cursor){		\
    uhstb_##t##_cursor* i = (uhstb_##t##_cursor*)cursor;	\
    i->next_idx = -1;						\
    i->next_nd = NULL;						\
  }

#define uhstb_cursor_next_tpl(t)					\
  static void* uhstb_##t##_cursor_next(uset* set,ucursor* cursor){	\
    uhstb_##t* hstb = (uhstb_##t*)set;					\
    uhstb_##t##_cursor* c = (uhstb_##t##_cursor*)cursor;		\
    void* k;								\
    while(!c->next_nd){							\
      if(hstb->len <= c->next_idx){					\
	return NULL;							\
      }else{								\
	c->next_idx++;							\
	c->next_nd = hstb->ndar[c->next_idx];				\
      }									\
    }									\
    k = (void*)&c->next_nd->k;						\
    c->next_nd = c->next_nd->next;					\
    return k;								\
  }

#define uhstb_new_tpl(t)				\
  uhstb_##t* uhstb_##t##_new(int len){			\
    uhstb_##t* hstb;					\
    int size = TYPE_SIZE_OF(uhstb_##t,uhsnd_##t,len);	\
    int i;						\
    unew(hstb,size,return NULL;);			\
    hstb->iterate = uhstb_##t##_cursor_init;		\
    hstb->next = uhstb_##t##_cursor_next;		\
    hstb->len = len;					\
    hstb->count = 0;					\
    hstb->pool = NULL;					\
    for(i = 0;i < len;i++){				\
      hstb->ndar[i] = NULL;				\
    }							\
    return hstb;					\
  }

#define uhstb_newmp_tpl(t)				\
  uhstb_##t* uhstb_##t##_newmp(umem_pool*pool,int len){	\
    uhstb_##t* hstb;					\
    int size = TYPE_SIZE_OF(uhstb_##t,uhsnd_##t,len);	\
    int i;						\
    hstb = umem_pool_alloc(pool,size);			\
    if (hstb) {						\
      hstb->iterate = uhstb_##t##_cursor_init;		\
      hstb->next = uhstb_##t##_cursor_next;		\
      hstb->len = len;					\
      hstb->count = 0;					\
      hstb->pool = pool;				\
      for(i = 0;i < len;i++){				\
	hstb->ndar[i] = NULL;				\
      }							\
    }							\
    return hstb;					\
  }

#define uhstb_put_tpl(t)					\
  int uhstb_##t##_put(uhstb_##t*          hstb,			\
		      unsigned int        hscd,			\
		      t*                  ink,			\
		      t**                 outk,			\
		      uhstb_##t##_key_ft  putk,			\
		      uhstb_##t##_comp_ft comp){		\
    uhsnd_##t* prev_nd = NULL;					\
    uhsnd_##t* nd = NULL;					\
    int idx = hscd%hstb->len;					\
    uhsnd_##t* ls = hstb->ndar[idx];				\
    while(ls){							\
      int c = comp(ink,&ls->k);					\
      if(c == 0){						\
	if(outk){						\
	  *outk = &ls->k;					\
	}							\
	return 1;						\
      }else if(c > 0){						\
	prev_nd = ls;						\
	ls = ls->next;						\
      }else{							\
	break;							\
      }								\
    }								\
    if (hstb->pool) {						\
      nd = umem_pool_alloc(hstb->pool,sizeof(uhsnd_##t));	\
    } else {							\
      nd = ualloc(sizeof(uhsnd_##t));				\
    }								\
    if (!nd) {							\
      return -1;						\
    }								\
    if(!prev_nd){						\
      hstb->ndar[idx] = nd;					\
    }else{							\
      prev_nd->next = nd;					\
    }								\
    if(putk){							\
      nd->k = putk(ink);					\
    }else{							\
      nd->k = *ink;						\
    }								\
    nd->next = ls;						\
    if(outk){							\
      *outk = &nd->k;						\
    }								\
    hstb->count++;						\
    return 0;							\
  }

#define uhstb_get_tpl(t)				\
  int uhstb_##t##_get(uhstb_##t*          hstb,		\
		      unsigned int        hscd,		\
		      t*                  ink,		\
		      t**                 outk,		\
		      uhstb_##t##_comp_ft comp){	\
    uhsnd_##t* ls = hstb->ndar[hscd%hstb->len];		\
    while(ls){						\
      int c = comp(ink,&ls->k);				\
      if(c == 0){					\
	*outk = &ls->k;					\
	return 0;					\
      }else if(c > 0){					\
	ls = ls->next;					\
      }else{						\
	break;						\
      }							\
    }							\
    *outk = NULL;					\
    return -1;						\
  }

#define uhstb_def_tpl(t)			\
  uhstb_cursor_tpl(t);				\
  uhstb_cursor_next_tpl(t);			\
  uhstb_new_tpl(t);				\
  uhstb_newmp_tpl(t);				\
  uhstb_put_tpl(t);				\
  uhstb_get_tpl(t)
