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

typedef struct _vinst{
  usize_t code;
  usize_t operand;
} vinst;

vinst*
vinst_new(usize_t code,usize_t operand);

int vinst_to_str(vcontext* ctx,
		 ulist* insts);

typedef struct _vsymbol{
  ustring* name;
  vslot slot;
} vsymbol;

#define vsymbol_init(name,index) \
  ((vsymbol){name,index})

vsymbol* vsymbol_new(ustring* name,usize_t index);

vcontext* vcontext_new(vgc_heap* heap);

void vcontext_execute(vcontext* ctx);

ustring* vcontext_str_put(vcontext* ctx,char* charp);

#endif
