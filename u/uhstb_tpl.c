#include <stddef.h>
#include <string.h>
#include "ualloc.h"
#include "uerror.h"
#include "uhstb_tpl.h"

#define uhstb_cursor_tpl(t)					\
  static void ucall uhstb_##t##_cursor_init(ucursor* cursor)	\
  {								\
    uhstb_##t##_cursor* i = (uhstb_##t##_cursor*)cursor;	\
    i->next_idx = -1;						\
    i->next_nd = NULL;						\
  }

#define uhstb_cursor_next_tpl(t)					\
  static void* ucall uhstb_##t##_cursor_next(uset* set,ucursor* cursor)	\
  {									\
    uhstb_##t* hstb = (uhstb_##t*)set;					\
    uhstb_##t##_cursor* c = (uhstb_##t##_cursor*)cursor;		\
    void* k;								\
    while(!c->next_nd){							\
      if(hstb->len <= c->next_idx + 1){					\
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

#define uhstb_map_put_tpl(t)						\
  static int ucall uhstb_map_##t##_put(umap_##t*           map,		\
				       unsigned int        hscd,	\
				       t*                  ink,		\
				       t**                 outk,	\
				       umap_##t##_key_ft   putk,	\
				       umap_##t##_comp_ft  comp)	\
  {									\
    return uhstb_##t##_put((uhstb_##t*)map,				\
			   hscd,					\
			   ink,						\
			   outk,					\
			   putk,					\
			   comp);					\
  }

#define uhstb_map_get_tpl(t)						\
  static int ucall uhstb_map_##t##_get(umap_##t*           map,		\
				       unsigned int        hscd,	\
				       t*                  ink,		\
				       t**                 outk,	\
				       umap_##t##_comp_ft  comp)	\
  {									\
    return uhstb_##t##_get((uhstb_##t*)map,				\
			   hscd,					\
			   ink,						\
			   outk,					\
			   comp);					\
  }

#define uhstb_new_tpl(t)				\
  uapi_tpl uhstb_##t* ucall uhstb_##t##_new(int len)	\
  {							\
    uallocator* allocator;				\
    							\
    allocator = &u_global_allocator;			\
    return uhstb_##t##_alloc(allocator,len);		\
  }

#define uhstb_alloc_tpl(t)					\
  uapi_tpl uhstb_##t* ucall					\
  uhstb_##t##_alloc(uallocator* allocator,int len)		\
  {								\
    uhstb_##t* hstb;						\
    int rlen;							\
    int size;							\
    int i;							\
    								\
    if (len > 0) {						\
      rlen = len;						\
    } else {							\
      rlen = UHSTB_LENGTH;					\
    }								\
    size = TYPE_SIZE_OF(uhstb_##t,uhsnd_##t,rlen);		\
    hstb = allocator->alloc(allocator,size);			\
    if (hstb) {							\
      hstb->iterate = uhstb_##t##_cursor_init;			\
      hstb->next = uhstb_##t##_cursor_next;			\
      hstb->put = uhstb_map_##t##_put;				\
      hstb->get = uhstb_map_##t##_get;				\
      hstb->len = rlen;						\
      hstb->count = 0;						\
      hstb->allocator = allocator;				\
      for(i = 0;i < rlen;i++){					\
	hstb->ndar[i] = NULL;					\
      }								\
    }								\
    return hstb;						\
  }

#define uhstb_dest_tpl(t)						\
  uapi_tpl void ucall							\
  uhstb_##t##_dest(uhstb_##t* hstb,uhstb_##t##_dest_ft dest)		\
  {									\
    int i;								\
    for (i = 0;i < hstb->len;i++) {					\
      uhsnd_##t* nd = hstb->ndar[i];					\
      while (nd) {							\
	uhsnd_##t* temp = nd;						\
	nd = nd->next;							\
	if (dest) {							\
	  dest(temp->k);						\
	}								\
	hstb->allocator->free(hstb->allocator,temp);			\
      }									\
    }									\
    hstb->allocator->free(hstb->allocator,hstb);			\
  }

#define uhstb_put_tpl(t)						\
  uapi_tpl int ucall							\
  uhstb_##t##_put(uhstb_##t*          hstb,				\
		  unsigned int        hscd,				\
		  t*                  ink,				\
		  t**                 outk,				\
		  uhstb_##t##_key_ft  putk,				\
		  uhstb_##t##_comp_ft comp)				\
  {									\
    uhsnd_##t* prev_nd = NULL;						\
    uhsnd_##t* nd = NULL;						\
    int idx = hscd%hstb->len;						\
    uhsnd_##t* ls = hstb->ndar[idx];					\
    while(ls){								\
      int c = comp(ink,&ls->k);						\
      if(c == 0){							\
	if(outk){							\
	  *outk = &ls->k;						\
	}								\
	return 1;							\
      }else if(c > 0){							\
	prev_nd = ls;							\
	ls = ls->next;							\
      }else{								\
	break;								\
      }									\
    }									\
    nd = hstb->allocator->alloc(hstb->allocator,sizeof(uhsnd_##t));	\
    if (!nd) {								\
      return -1;							\
    }									\
    if(!prev_nd){							\
      hstb->ndar[idx] = nd;						\
    }else{								\
      prev_nd->next = nd;						\
    }									\
    if(putk){								\
      nd->k = putk(ink);						\
    }else{								\
      nd->k = *ink;							\
    }									\
    nd->next = ls;							\
    if(outk){								\
      *outk = &nd->k;							\
    }									\
    hstb->count++;							\
    return 0;								\
  }

#define uhstb_get_tpl(t)				\
  uapi_tpl int ucall					\
  uhstb_##t##_get(uhstb_##t*          hstb,		\
		  unsigned int        hscd,		\
		  t*                  ink,		\
		  t**                 outk,		\
		  uhstb_##t##_comp_ft comp)		\
  {							\
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
  uhstb_map_put_tpl(t);				\
  uhstb_map_get_tpl(t);				\
  uhstb_new_tpl(t);				\
  uhstb_alloc_tpl(t);				\
  uhstb_dest_tpl(t);				\
  uhstb_put_tpl(t);				\
  uhstb_get_tpl(t)
