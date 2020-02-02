#ifndef _VPASS_H_
#define _VPASS_H_

#include "ulist.h"
#include "vgc_obj.h"

typedef struct _vbb{
  ulist* insts;
} vbb;

typedef struct _vdfg{
  ulist* bbs;
  vbb* entry;
} vdfg;

vgc_string* vpass_dfg2bin(vdfg* dfg);

#endif
