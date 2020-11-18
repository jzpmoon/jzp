#ifndef _UHASH_TABLE_H_
#define _UHASH_TABLE_H_

#include "udef.h"
#include "uhstb_tpl.h"

uhstb_decl_tpl(int);
uhstb_decl_tpl(long);
uhstb_decl_tpl(float);
uhstb_decl_tpl(double);
uhstb_decl_tpl(uvoidp);

typedef struct _uhash_node{
	struct _uhash_node* next;
	void*               k;
} uhash_node,* uhash_table;

typedef void*(*uhstb_key_ft)(void*);
typedef int(*uhstb_comp_ft)(void*,void*);

uhash_table* uhash_table_new(int size);

void* uhash_table_put(uhash_table*  hstb,
		      unsigned int  hscd,
		      void*         k,
		      uhstb_key_ft  putk,
		      uhstb_comp_ft comp);

void* uhash_table_get(uhash_table*  hstb,
		      unsigned int  hscd,
		      void*         k,
		      uhstb_key_ft  getk,
		      uhstb_comp_ft comp);
#endif
