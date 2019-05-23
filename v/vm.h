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

vgc_str*
vinst_to_str(vgc_heap* heap,
	     ulist* insts);

typedef struct _vcontext{
  vgc_heap* heap;
  vgc_stack* stack;
  vgc_call* curr_call;
} vcontext;

vcontext*
vcontext_new(vgc_heap* heap);

void vcontext_execute(vcontext* ctx,
		      vgc_subr* subr);
#endif
