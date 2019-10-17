#ifndef _VVM_H_
#define _VVM_H_

#include "ulist.h"
#include "vgc_obj.h"
#include "vbyte_code.h"

enum vbyte_code{
  #define DF(code,name,value,len) \
    code=value,
    VBYTE_CODE
  #undef DF
};

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
vcontext_new(struct _vvm* vm,usize_t stack_size);

void vcontext_execute(vcontext* ctx,
		      vslot*    slotp_entry);

vslot* vcontext_args_get(vcontext* ctx,usize_t index);

void vcontext_cache_clean(vcontext* ctx);

#define VVM_SYMTB_SIZE 17

typedef struct _vvm{
  vgc_heap*    heap;
  uhash_table* symtb;
  vgc_stack*   consts;
  vcontext*    context;
} vvm;

vvm* vvm_new(usize_t static_size,
	     usize_t active_size,
	     usize_t stack_size,
	     usize_t consts_size);

int vvm_obj_put(vvm* vm,ustring* name,vslot* obj);

int vvm_obj_get(vvm* vm,ustring* name);

#endif
