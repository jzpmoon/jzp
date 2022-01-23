#ifndef _VCONTEXT_H_
#define _VCONTEXT_H_

#include "ustring.h"
#include "vgc_obj.h"
#include "vpass.h"
#include "vgenbc.h"

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

#define VMOD_STATUS_LOADED 1
#define VMOD_STATUS_UNLOAD 0

typedef struct _vmod{
  ulist_vreloc* rells;
  uhstb_vsymbol* gobjtb;
  uhstb_vsymbol* lobjtb;
  struct _vgc_subr* init;
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
  ustack_vslot* stack;
  umem_pool mp;
  uhstb_vmod* mods;
  vmod_loader* loader;
  ustring_table* symtb;
  ustring_table* strtb;
  vslot_define_begin
    vslot_define(vgc_call,calling);
  vslot_define_end
};

#define vcontext_alloc_get(ctx) \
  &(ctx)->mp.allocator

typedef struct _vgc_subr{
  VGCHEADER;
  usize_t params_count;
  usize_t locals_count;
  vslot_define_begin
    vslot_define(vgc_array,consts);
    vslot_define(vgc_string,bytecode);
  vslot_define_end
} vgc_subr;

vgc_subr* vgc_subr_new(vcontext* ctx,
		       usize_t params_count,
		       usize_t locals_count,
		       int area_type);

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

enum {
  vgc_call_type_cfun,
  vgc_call_type_subr,
};

typedef struct _vgc_call{
  VGCHEADER;
  int call_type;
  usize_t base;
  unsigned char* pc;
  vslot_define_begin
    vslot_define(vgc_subr,cfun);
    vslot_define(vgc_subr,subr);
    vslot_define(vgc_call,caller);
  vslot_define_end
} vgc_call;

vgc_call* vgc_call_new(vcontext* ctx,
		       int call_type,
		       usize_t base);

#define vgc_call_is_cfun(call)				\
  ((call)->call_type == vgc_call_type_cfun)

UDECLFUN(UFNAME vcontext_new,UARGS (vgc_heap* heap),URET vcontext*);

vslot vcontext_params_get(vcontext* ctx,int index);

UDECLFUN(UFNAME vcontext_vps_load,
	 UARGS (vcontext* ctx,vps_cntr* vps),
	 URET int);

UDECLFUN(UFNAME vcontext_mod_load,
	 UARGS (vcontext* ctx,vps_mod* mod),
	 URET int);

UDECLFUN(UFNAME vcontext_graph_load,
	 UARGS (vcontext* ctx,vmod* mod,vcfg_graph* grp),
	 URET vgc_subr*);

void vcontext_execute(vcontext* ctx);

void vcontext_relocate(vcontext* ctx);

vmod* vcontext_mod_add(vcontext* ctx,ustring* name);

UDECLFUN(UFNAME vcontext_mod2mod,
	 UARGS (vcontext* ctx,vmod* dest_mod,vps_mod* src_mod),
	 URET int);

void vmod_add_reloc(vmod* mod,vreloc reloc);

vsymbol* vmod_gobj_put(vgc_heap* heap,vmod* mod,ustring* name,vgc_obj* obj);

vsymbol* vmod_lobj_put(vgc_heap* heap,vmod* mod,ustring* name,vgc_obj* obj);

vsymbol* vmod_gslot_put(vgc_heap* heap,vmod* mod,ustring* name,vslot slot);

vsymbol* vmod_lslot_put(vgc_heap* heap,vmod* mod,ustring* name,vslot slot);

vslot vcontext_stack_get(vcontext* ctx,usize_t index);

void vcontext_stack_set(vcontext* ctx,usize_t index,vslot slot);

usize_t vcontext_stack_top_get(vcontext* ctx);

void vcontext_stack_top_set(vcontext* ctx,usize_t index);

#define vcontext_stack_push(ctx,slot)			\
  if(ustack_vslot_push(ctx->stack,slot)){		\
    uabort("vgc_heap_stack: overflow!");		\
  }

#define vcontext_stack_pop(ctx,slotp)			\
  if(ustack_vslot_pop(ctx->stack,slotp)){		\
    uabort("vgc_heap_stack: empty!");			\
  }

#define vcontext_stack_pushv(ctx)		\
  if(ustack_vslot_pushv(ctx->stack)){		\
    uabort("vgc_heap_stack: overflow!");	\
  }

#define vcontext_obj_push(ctx,obj)		\
  do{						\
    vslot __slot;				\
    vslot_ref_set(__slot,obj);			\
    vcontext_stack_push(ctx,__slot);		\
  } while(0)

#define vcontext_obj_pop(ctx,obj,obj_type)	\
  do{						\
    vslot __slot;				\
    vcontext_stack_pop(ctx,&__slot);		\
    obj = vslot_ref_get(__slot,obj_type);	\
  } while(0)

#define vcontext_obj_slot_get(ctx,obj,slot)		\
  vcontext_stack_push(ctx,(obj)->_u.slot);
  
#define vcontext_obj_slot_set(ctx,obj,slot)		\
  vcontext_stack_pop(ctx,&((obj)->_u.slot));

#endif
