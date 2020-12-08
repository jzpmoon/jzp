#ifndef _VCONTEXT_H_
#define _VCONTEXT_H_

#include "ustring.h"
#include "vgc_obj.h"
#include "vpass.h"

vcontext* vcontext_new(vgc_heap* heap);

vsymbol* vcontext_obj_put(vcontext* ctx,ustring* name,vgc_obj* obj);

vsymbol* vcontext_slot_put(vcontext* ctx,ustring* name,vslot obj);

vsymbol* vcontext_symbol_get(vcontext* ctx,ustring* name);

vsymbol* vcontext_graph_load(vcontext* ctx,vdfg_graph* grp);

int vcontext_load(vcontext* ctx,vps_t* vps);

void vcontext_execute(vcontext* ctx);

void vcontext_relocate(vcontext* ctx);

#endif
