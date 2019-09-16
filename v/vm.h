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

#define VINST_C(L,C) vinst_new(L,vinst_new(C,-1))

#define VINST_CO(L,C,O) vinst_new(L,vinst_new(C,O))

typedef struct _vinst{
  usize_t code;
  usize_t operand;
} vinst;

vinst*
vinst_new(usize_t code,usize_t operand);

vgc_str*
vinst_to_str(vgc_heap* heap,
	     ulist* insts);

typedef struct _vcontext{
  VGCHEADER;
  vgc_heap* heap;
  /*vgc_stack*/
  vslot stack;
  /*vgc_call*/
  vslot curr_call;
} vcontext;

vcontext*
vcontext_new(vgc_heap* heap,usize_t stack_size);

void vcontext_execute(vcontext* ctx,
		      vgc_subr* subr);
#endif
