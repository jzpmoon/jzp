#ifndef _VGENBC_H_
#define _VGENBC_H_

#include "vbytecode.h"
#include "vgc_obj.h"
#include "vpass.h"

enum vbytecode{
  #define DF(code,name,value,len,oct) \
    code=value,
    VBYTECODE
  #undef DF
};

#define vopdnon (-1)

vgc_string* vps_inst_to_str(vgc_heap* heap,
			    ulist_vps_instp* insts);

#endif
