#ifndef _VCONTEXT_H_
#define _VCONTEXT_H_

#include "ustring.h"
#include "vgc_obj.h"
#include "vpass.h"

vcontext* vcontext_new(vgc_heap* heap);

vslot vcontext_params_get(vcontext* ctx,int index);

int vcontext_mod_load(vcontext* ctx,vps_mod* mod);

int vcontext_data_load(vcontext* ctx,vps_data* data);

vsymbol* vcontext_graph_load(vcontext* ctx,vmod* mod,vdfg_graph* grp);

void vcontext_execute(vcontext* ctx);

void vcontext_relocate(vcontext* ctx);

void vcontext_mod_add(vcontext* ctx,vmod mod);

void vmod_init(vmod* mod);

void vmod_add_reloc(vmod* mod,vreloc reloc);

vsymbol* vmod_gobj_put(vmod* mod,ustring* name,vgc_obj* obj);

vsymbol* vmod_lobj_put(vmod* mod,ustring* name,vgc_obj* obj);

#endif
