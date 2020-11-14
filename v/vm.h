#ifndef _VM_H_
#define _VM_H_

#include "ulist.h"
#include "vgc_obj.h"
#include "vbytecode.h"
#include "vcontext.h"

enum vbytecode{
  #define DF(code,name,value,len) \
    code=value,
    VBYTECODE
  #undef DF
};

void vcontext_execute(vcontext* ctx);

#endif
