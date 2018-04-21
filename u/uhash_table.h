#ifndef _UHASH_TABLE_H_
#define _UHASH_TABLE_H_

typedef struct _uhash_node{
	struct _uhash_node* next;
	void* k;
	void* v;
} uhash_node,* uhash_table;

typedef int(*hstb_comp)(void*,void*);

void* uhash_table_put(
	uhash_table* hstb,
	unsigned int hscd,
	void* k,
	void* v,
	hstb_comp comp);

void* uhash_table_get(
	uhash_table* hstb,
	unsigned int hscd,
	void* k,
	hstb_comp comp);
#endif
