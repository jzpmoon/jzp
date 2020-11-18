#include <stddef.h>
#include <string.h>
#include "ualloc.h"
#include "uhash_table.h"
#include "uhstb_tpl.c"

uhstb_def_tpl(int);
uhstb_def_tpl(long);
uhstb_def_tpl(float);
uhstb_def_tpl(double);
uhstb_def_tpl(uvoidp);

uhash_table* uhash_table_new(int size){
  uhash_table* hstb;
  int _size = sizeof(uhash_node) * size;
  unew(hstb,_size,return NULL;);
  memset(hstb,0,_size);
  return hstb;
}

void* uhash_table_put(uhash_table*  hstb,
		      unsigned int  hscd,
		      void*         k,
		      uhstb_key_ft  putk,
		      uhstb_comp_ft comp){
  uhash_node* prev_nd = NULL;
  uhash_node* nd      = NULL;
  uhash_node* ls      = hstb[hscd];
  while(ls){
    int c = comp(k,ls->k);
    if(c == 0){
      return ls->k;
    }else if(c > 0){
      prev_nd = ls;
      ls = ls->next;
    }else{
      break;
    }
  }
  unew(nd,sizeof(uhash_node),return NULL;);
  if(!prev_nd){
    hstb[hscd] = nd;
  }else{
    prev_nd->next = nd;
  }
  if(putk){
    nd->k = putk(k);
  }
  nd->next = ls;
  return nd->k;
}

void* uhash_table_get(uhash_table*  hstb,
		      unsigned int  hscd,
		      void*         k,
		      uhstb_key_ft  getk,
		      uhstb_comp_ft comp){
  uhash_node* ls = hstb[hscd];
  while(ls){
    int c = comp(k,ls->k);
    if(c == 0){
      if(getk){
	return getk(ls->k);
      }else{
	return ls->k;
      }
    }else if(c > 0){
      ls = ls->next;
    }else{
      break;
    }
  }
  return NULL;
}
