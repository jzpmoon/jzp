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

typedef struct _vsymbol{
  ustring* name;
  vslot slot;
} vsymbol;

vcontext* vcontext_new(vgc_heap* heap);

void vcontext_execute(vcontext* ctx);

vsymbol* vcontext_obj_put(vcontext* ctx,ustring* name,vslot obj);

#endif
