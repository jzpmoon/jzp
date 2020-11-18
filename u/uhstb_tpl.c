#include <stddef.h>
#include <string.h>
#include "ualloc.h"
#include "uhstb_tpl.h"

#define uhstb_new_tpl(t)			\
  uhstb_##t* uhstb_##t##_new(int size){		\
    uhstb_##t* hstb;				\
    int _size = sizeof(uhsnd_##t) * size;	\
    unew(hstb,_size,return NULL;);		\
    memset(hstb,0,_size);			\
    return hstb;				\
  }

#define uhstb_put_tpl(t)				\
  int uhstb_##t##_put(uhstb_##t*          hstb,		\
		      unsigned int        hscd,		\
		      t                   ink,		\
		      t                   outk,		\
		      uhstb_##t##_key_ft  putk,		\
		      uhstb_##t##_comp_ft comp){	\
    uhsnd_##t* prev_nd = NULL;				\
    uhsnd_##t* nd      = NULL;				\
    uhsnd_##t* ls      = hstb[hscd];			\
    while(ls){						\
      int c = comp(ink,ls->k);				\
      if(c == 0){					\
	outk = ls->k;					\
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
      hstb[hscd] = nd;					\
    }else{						\
      prev_nd->next = nd;				\
    }							\
    if(putk){						\
      nd->k = putk(ink);				\
    }							\
    nd->next = ls;					\
    outk = nd->k;					\
    return 0;						\
  }

#define uhstb_get_tpl(t)				\
  int uhstb_##t##_get(uhstb_##t*          hstb,		\
		      unsigned int        hscd,		\
		      t                   ink,		\
		      t                   outk,		\
		      uhstb_##t##_key_ft  getk,		\
		      uhstb_##t##_comp_ft comp){	\
    uhsnd_##t* ls = hstb[hscd];				\
    while(ls){						\
      int c = comp(ink,ls->k);				\
      if(c == 0){					\
	if(getk){					\
	  outk = getk(ls->k);				\
	  return 0;					\
	}else{						\
	  outk = ls->k;					\
	  return 0;					\
	}						\
      }else if(c > 0){					\
	ls = ls->next;					\
      }else{						\
	break;						\
      }							\
    }							\
    return -1;						\
  }

#define uhstb_def_tpl(t)			\
  uhstb_new_tpl(t);				\
  uhstb_put_tpl(t);				\
  uhstb_get_tpl(t);
