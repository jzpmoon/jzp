#ifndef _VGC_H_
#define _VGC_H_

#include "vobj.h"


typedef struct _VgcHeap{
  usize_t size;
  VgcRootSet* rootSet;
  VgcObj* obj;
  void* addr;
} VgcHeap;

void gcMark(VgcObj* root,VgcObj* heap);
void gcSweep(VgcObj* heep);
VgcObj* vgcAlloc(usize_t s,
		 enum gcObj_t t,
		 VgcRootSet* rootSet,
		 VgcObj* heap);

#endif
