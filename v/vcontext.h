#ifndef _VCONTEXT_H_
#define _VCONTEXT_H_

#include "ustring.h"
#include "vgc_obj.h"
#include "vpass.h"

vcontext* vcontext_new(vgc_heap* heap);

vsymbol* vcontext_obj_put(vcontext* ctx,ustring* name,vslot obj);

int vcontext_load(vcontext* ctx,vps_t* vps);

#endif
