#ifndef _VM_H_
#define _VM_H_

#include "ulist.h"
#include "vgc_obj.h"
#include "vbyte_code.h"

enum vbyte_code{
  #define DF(code,name,value,len) \
    code=value,
    VBYTE_CODE
  #undef DF
};

#define VINSTC(L,C) vinst_new(L,vinst_new(C,-1))

#define VINSTCO(L,C,O) vinst_new(L,vinst_new(C,O))

typedef struct _vinst{
  usize_t code;
  usize_t operand;
} vinst;

vinst*
vinst_new(usize_t code,usize_t operand);

vslot*
vinst_to_str(vcontext* ctx,
	     ulist*    insts);

typedef struct _vsymbol{
  ustring* name;
  usize_t  index;
} vsymbol;

#define vsymbol_init(name,index) \
  ((vsymbol){name,index})

vsymbol* vsymbol_new(ustring* name,usize_t index);

vcontext*
vcontext_new(struct _vm* vm,usize_t stack_size);

void vcontext_execute(vcontext* ctx,
		      vslot*    slotp_entry);

vslot* vcontext_args_get(vcontext* ctx,usize_t index);

void vcontext_cache_clean(vcontext* ctx);

#define VM_SYMTB_SIZE 17

typedef struct _vm{
  vgc_heap*    heap;
  uhash_table* symtb;
  vgc_stack*   consts;
  vcontext*    context;
} vm;

vm* vm_new(usize_t static_size,
	   usize_t active_size,
	   usize_t stack_size,
	   usize_t consts_size);

int vm_obj_put(vm* vm,ustring* name,vslot* obj);

int vm_obj_get(vm* vm,ustring* name);

#endif
