#ifndef _VCONTEXT_H_
#define _VCONTEXT_H_

#include "ustring.h"
#include "vgc_obj.h"
#include "vpass.h"

typedef struct _vreloc{
  ustring* ref_name;
  vgc_array* rel_obj;
  int rel_idx;
} vreloc;

typedef struct _vsymbol{
  ustring* name;
  vslot slot;
} vsymbol;

#define vreloc_log(reloc)				\
  ulog("ref_name:%s",reloc->ref_name->value);		\
  ulog("rel_obj:%p",reloc->rel_obj);			\
  ulog("rel_idx:%d",reloc->rel_idx);

ulist_decl_tpl(vreloc);
uhstb_decl_tpl(vsymbol);

#define VCONTEXT_SYMTB_SIZE 17
#define VCONTEXT_STRTB_SIZE 17

#define VMOD_STATUS_LOADED 1
#define VMOD_STATUS_UNLOAD 0

typedef struct _vmod{
  ulist_vreloc* rells;
  uhstb_vsymbol* gobjtb;
  uhstb_vsymbol* lobjtb;
  vgc_subr* init;
  ustring* name;
  int status;
} vmod;

#define vmod_loaded(mod)			\
  ((mod)->status = VMOD_STATUS_LOADED)
#define vmod_isload(mod)			\
  ((mod)->status == VMOD_STATUS_LOADED)

uhstb_decl_tpl(vmod);

typedef struct _vmod_loader vmod_loader;

typedef int(*vmod_load_ft)(vmod_loader* loader,vmod* mod);

#define VMOD_LOADER_HEADER \
  vmod_load_ft load;	   \

struct _vmod_loader{
  VMOD_LOADER_HEADER;
};

typedef struct _vcontext vcontext;

struct _vcontext{
  VGCHEADER;
  vgc_heap* heap;
  umem_pool mp;
  uhstb_vmod* mods;
  vmod_loader* loader;
  ustring_table* symtb;
  ustring_table* strtb;
  vslot_define_begin
    vslot_define(vgc_call,calling);
  vslot_define_end
};

typedef int(*vcfun_ft)(vcontext*);

typedef struct _vgc_cfun{
  VGCHEADER;
  vcfun_ft entry;
  int params_count;
  int has_retval;
  vslot_define_begin
  /*void member*/
  vslot_define_end
} vgc_cfun;

vgc_cfun* vgc_cfun_new(vgc_heap* heap,
		       vcfun_ft entry,
		       int params_count,
		       int has_retval,
		       int area_type);

UDECLFUN(UFNAME vcontext_new,UARGS (vgc_heap* heap),URET vcontext*);

vslot vcontext_params_get(vcontext* ctx,int index);

int vcontext_vps_load(vcontext* ctx,vps_cntr* vps);

int vcontext_mod_load(vcontext* ctx,vps_mod* mod);

vgc_subr* vcontext_graph_load(vcontext* ctx,vmod* mod,vdfg_graph* grp);

void vcontext_execute(vcontext* ctx);

void vcontext_relocate(vcontext* ctx);

vmod* vcontext_mod_add(vcontext* ctx,ustring* name);

int vcontext_mod2mod(vcontext* ctx,vmod* dest_mod,vps_mod* src_mod);

void vmod_add_reloc(vmod* mod,vreloc reloc);

vsymbol* vmod_gobj_put(vgc_heap* heap,vmod* mod,ustring* name,vgc_obj* obj);

vsymbol* vmod_lobj_put(vgc_heap* heap,vmod* mod,ustring* name,vgc_obj* obj);

vsymbol* vmod_gslot_put(vgc_heap* heap,vmod* mod,ustring* name,vslot slot);

vsymbol* vmod_lslot_put(vgc_heap* heap,vmod* mod,ustring* name,vslot slot);

#endif
