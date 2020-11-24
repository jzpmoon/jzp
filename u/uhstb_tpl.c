#include <stddef.h>
#include <string.h>
#include "ualloc.h"
#include "uerror.h"
#include "uhstb.h"

#define uhstb_iterator_tpl(t)					\
  static void uhstb_##t##_iterator_init(uiterator* iterator){	\
    uhstb_##t##_iterator* i = (uhstb_##t##_iterator*)iterator;	\
    i->next_idx = -1;						\
    i->next_nd = NULL;						\
  }

#define uhstb_next_tpl(t)					\
  static void* uhstb_##t##_next(uset* set,uiterator* iterator){	\
    uhstb_##t* hstb = (uhstb_##t*)set;				\
    uhstb_##t##_iterator* i = (uhstb_##t##_iterator*)iterator;	\
    void* k;							\
    while(!i->next_nd){						\
      if(hstb->len <= i->next_idx){				\
	return NULL;						\
      }else{							\
	i->next_idx++;						\
	i->next_nd = hstb->ndar[i->next_idx];			\
      }								\
    }								\
    k = (void*)&i->next_nd->k;					\
    i->next_nd = i->next_nd->next;				\
    return k;							\
  }

#define uhstb_new_tpl(t)				\
  uhstb_##t* uhstb_##t##_new(int len){			\
    uhstb_##t* hstb;					\
    int size = TYPE_SIZE_OF(uhstb_##t,uhsnd_##t,len);	\
    int i;						\
    unew(hstb,size,return NULL;);			\
    hstb->iterate = uhstb_##t##_iterator_init;		\
    hstb->next = uhstb_##t##_next;			\
    hstb->len = len;					\
    for(i = 0;i < len;i++){				\
      hstb->ndar[i] = NULL;				\
    }							\
    return hstb;					\
  }

#define uhstb_put_tpl(t)				\
  int uhstb_##t##_put(uhstb_##t*          hstb,		\
		      unsigned int        hscd,		\
		      t*                  ink,		\
		      t**                 outk,		\
		      uhstb_##t##_key_ft  putk,		\
		      uhstb_##t##_comp_ft comp){	\
    uhsnd_##t* prev_nd = NULL;				\
    uhsnd_##t* nd = NULL;				\
    int idx = hscd%hstb->len;				\
    uhsnd_##t* ls = hstb->ndar[idx];			\
    while(ls){						\
      int c = comp(ink,&ls->k);				\
      if(c == 0){					\
	if(outk){					\
	  *outk = &ls->k;				\
	}						\
	return 0;					\
      }else if(c > 0){					\
	prev_nd = ls;					\
	ls = ls->next;					\
      }else{						\
	break;						\
      }							\
    }							\
    unew(nd,sizeof(uhsnd_##t),return -1;);		\
    if(!prev_nd){					\
      hstb->ndar[idx] = nd;				\
    }else{						\
      prev_nd->next = nd;				\
    }							\
    if(putk){						\
      nd->k = *putk(ink);				\
    }else{						\
      nd->k = *ink;					\
    }							\
    nd->next = ls;					\
    if(outk){						\
      *outk = &nd->k;					\
    }							\
    return 0;						\
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
  uhstb_iterator_tpl(t);			\
  uhstb_next_tpl(t);				\
  uhstb_new_tpl(t);				\
  uhstb_put_tpl(t);				\
  uhstb_get_tpl(t)
