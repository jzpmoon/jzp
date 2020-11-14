#ifndef _VGENBC_H_
#define _VGENBC_H_

#include "ulist_tpl.h"
#include "vbytecode.h"
#include "vgc_obj.h"

enum vbytecode{
  #define DF(code,name,value,len) \
    code=value,
    VBYTECODE
  #undef DF
};

#define vopdnon (-1)

typedef struct _vinst{
  usize_t opcode;
  usize_t operand;
} vinst,*vinstp;

ulist_decl_tpl(vinstp);

vgc_string* vinst_to_str(vgc_heap* heap,
			 ulist_vinstp* insts);

#endif
