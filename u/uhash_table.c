#include <stddef.h>
#include <string.h>
#include "ualloc.h"
#include "uhash_table.h"

void* uhash_table_put(
	uhash_table* hstb,
	unsigned int hscd,
	void* k,
	void* v,
	hstb_comp comp){
	uhash_node* ls=NULL;
	uhash_node* prev_nd=NULL;
	uhash_node* nd=NULL;
	ls=hstb[hscd];
	while(ls!=NULL){
		int c=comp(k,ls->k);
		if(c==0){
			ls->v=v;
			return ls->k;
		}else if(c>0){
			prev_nd=ls;
			ls=ls->next;
		}else{
			break;
		}
	}
	unew(nd,
	     sizeof(uhash_node),
	     return NULL;)
	if(prev_nd==NULL){
		hstb[hscd]=nd;
	}else{
		prev_nd->next=nd;
	}
	nd->k=k;
	nd->v=v;
	nd->next=ls;
	return nd->k;
}

void* uhash_table_get(
	uhash_table* hstb,
	unsigned int hscd,
	void* k,
	hstb_comp comp){
	uhash_node* ls=hstb[hscd];
	while(ls!=NULL){
		int c=comp(k,ls->k);
		if(c==0){
			return ls->v;
		}else if(c>0){
			ls=ls->next;
		}else{
			break;
		}
	}
	return NULL;
}
