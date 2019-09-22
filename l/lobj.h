#ifndef _LOBJ_H_
#define _LOBJ_H_

#include "vgc_obj.h"

typedef struct _lcons{
  VGCHEADEREX
  vgc_obj* car;
  vgc_obj* cdr;
} lcons;

lcons* lcons_new(vgc_heap* heap);

#endif
