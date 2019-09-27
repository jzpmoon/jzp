#ifndef _LOBJ_H_
#define _LOBJ_H_

#include "vgc_obj.h"

typedef struct _lcons{
  VGCHEADEREX
  vslot car;
  vslot cdr;
} lcons;

typedef struct _lsymbol{
  VGCHEADEREX
  ustring* key;
  vslot    val;
} lsymbol;

lcons* lcons_new(vgc_heap* heap);

lsymbol* lsymbol_new(vgc_heap* heap,ustring* key);

#endif
