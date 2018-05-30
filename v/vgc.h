#ifndef _VGC_H_
#define _VGC_H_

#include "vobj.h"


typedef struct _VgcHeap{
  usize_t size;
  VgcRootSet* rootSet;
  VgcObj* alive;
  VgcObj* limit;
  void* addr;
} VgcHeap;

usize_t vgcObjSize(VgcObj* obj);

#endif
