#include "vgc.h"

static int try_alloc(VgcHeap* heap){
  return 0;
}

usize_t vgcObjSize(VgcObj* obj){
  switch(obj->t){
#define objt_def(t,s) case t:{return s(obj);}
    gcobjt_defs
#undef objt_def
  default: return 0;
  }
}
