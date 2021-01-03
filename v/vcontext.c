#include "uhstb_tpl.c"
#include "ulist_tpl.c"
#include "vcontext.h"

ulist_def_tpl(vreloc);
ulist_def_tpl(vmod);
uhstb_def_tpl(vsymbol);

#define VCONTEXT_OBJTB_SIZE 17
#define VCONTEXT_SYMTB_SIZE 17
#define VCONTEXT_STRTB_SIZE 17
#define VCONTEXT_CONSTS_SIZE 20

vcontext* vcontext_new(vgc_heap* heap){
  vcontext* ctx;
  ulist_vmod* mods;
  ustring_table* symtb;
  ustring_table* strtb;
  vgc_array* consts;

  ctx = vgc_heap_obj_new(heap,
			 vcontext,
			 vgc_obj_type_ctx,
			 vgc_heap_area_static);

  if(!ctx){
    uabort("vcontext_new: ctx new error!");
  }

  mods = ulist_vmod_new();
  if (!mods) {
    uabort("vcontext_new:mods new error!");
  }
    
  symtb = ustring_table_new(VCONTEXT_SYMTB_SIZE);
  if(!symtb){
    uabort("vcontext_new:symtb new error!");
  }

  strtb = ustring_table_new(VCONTEXT_STRTB_SIZE);
  if(!strtb){
    uabort("vcontext_new:strtb new error!");
  }
  consts = vgc_array_new(heap,
			 VCONTEXT_CONSTS_SIZE,
			 vgc_heap_area_static);
  if(!consts){
    uabort("vcontext_new:consts new error!");
  }

  ctx->heap = heap;
  ctx->mods = mods;
  ctx->symtb = symtb;
  ctx->strtb = strtb;
  vgc_obj_null_set(ctx,calling);
  vgc_obj_ref_set(ctx,consts,consts);
  return ctx;
}

vsymbol* vsymbol_new(ustring* name,vslot slot){
  vsymbol* symbol = ualloc(sizeof(vsymbol));
  if(symbol){
    symbol->name = name;
    symbol->slot = slot;
  }
  return symbol;
}

static int vobjtb_key_comp(vsymbol* sym1,vsymbol* sym2){
  return ustring_comp(sym1->name, sym2->name);
}

vsymbol* vcontext_graph_load(vcontext* ctx,vmod* mod,vdfg_graph* grp){
  vsymbol* symbol;
  ulist_vps_dfgp* dfgs = grp->dfgs;
  ulist_vinstp* insts = ulist_vinstp_new();
  vgc_subr* subr;
  ucursor cursor;

  dfgs->iterate(&cursor);
  while(1){
    vps_dfgp* dfgp = dfgs->next((uset*)dfgs,&cursor);
    vdfg_block* blk;
    if(!dfgp){
      break;
    }
    if((*dfgp)->t != vdfgk_blk){
      uabort("vps_dfg not a block!");
    }
    blk = (vdfg_block*)(*dfgp);
    if(vdfg_blk2inst(ctx,mod,blk,insts)){
      uabort("vdfg_blk2inst error!");
    }
  }

  vinst_to_str(ctx,insts);
  vgc_obj_slot_get(ctx->heap,ctx,consts);
  subr = vgc_subr_new(ctx->heap,
		      grp->params_count,
		      grp->locals_count,
		      vgc_heap_area_active);
  symbol = vmod_lobj_put(mod,grp->name,(vgc_obj*)subr);
  vmod_gobj_put(mod,grp->name,(vgc_obj*)subr);
  ulist_vinstp_del(insts,NULL);
  ulog("vcontext_graph_load");
  return symbol;
}

int vcontext_mod_load(vcontext* ctx,vps_mod* mod)
{
  vmod ctx_mod;
  ucursor cursor;
  uhstb_vps_datap* data = mod->data;
  uhstb_vdfg_graphp* code = mod->code;
  vsymbol* symbol;
    
  ulog("vcontext_load mod");

  vmod_init(&ctx_mod);
  vcontext_mod_add(ctx,ctx_mod);
    
  (data->iterate)(&cursor);
  ulog("vcontext_load mod data");
  while(1){
    vps_datap* dp = (data->next)((uset*)data,&cursor);
    if(!dp){
      break;
    }
    ulog("vcontext_load mod data entry");
    vcontext_data_load(ctx,*dp);
  }

  ulog("vcontext_load mod code");
  (data->iterate)(&cursor);
  while(1){
    vdfg_graphp* gp = (code->next)((uset*)code,&cursor);
    vdfg_graph* g;
    if(!gp){
      break;
    }
    g = *gp;
    ulog1("vcontext_load mod graph: %s",g->name->value);
    vcontext_graph_load(ctx,&ctx_mod,g);
  }
  symbol = vcontext_graph_load(ctx,&ctx_mod,mod->entry);
  /* 
   * 1. finish load mod
   * 2. clean vps memory
   * 3. execute mod entry
   */
  vps_cntr_clean(mod->vps);
  vcontext_relocate(ctx);
  vgc_heap_stack_push(ctx->heap,symbol->slot);
  vcontext_execute(ctx);
  return 0;
}

int vcontext_data_load(vcontext* ctx,vps_data* data)
{
  vgc_array* consts = vgc_obj_ref_get(ctx,consts,vgc_array);
  vslot slot;
  int top;

  if(data->dtk == vdtk_num){
    vslot_num_set(slot,data->u.number);      
  }else if(data->dtk == vdtk_int){
    vslot_int_set(slot,data->u.integer);
  }else if(data->dtk == vdtk_arr){
    
    return 0;
  }else if(data->dtk == vdtk_any){
    vslot_null_set(slot);
  }else{
    uabort("vcontext_load unknow data type");
  }
  top = vgc_array_push(consts,slot);
  if(top == -1){
    uabort("vcontext_load consts overflow!");
  }
  data->idx = top;
  ulog1("vcontext_load data:%s",data->name->value);
  ulog1("vcontext_load data idx:%d",top);
  return 0;
}

vsymbol* vmod_symbol_get(vcontext* ctx,vmod* mod,ustring* name){
  vsymbol symbol_in;
  vsymbol* symbol_out;
  ulist_vmod* mods;
  ucursor cursor;

  symbol_in.name = name;
  uhstb_vsymbol_get(mod->lobjtb,
		    name->hash_code,
		    &symbol_in,
		    &symbol_out,
		    vobjtb_key_comp);
  if (symbol_out) {
    return symbol_out;
  }
  mods = ctx->mods;
  mods->iterate(&cursor);
  while (1) {
    vmod* next = mods->next((uset*)mods,&cursor);
    if (!next) {
      break;
    }
    uhstb_vsymbol_get(next->gobjtb,
		      name->hash_code,
		      &symbol_in,
		      &symbol_out,
		      vobjtb_key_comp);
    if (symbol_out) {
      break;
    }
  }
  return symbol_out;
}

void vmod_relocate(vcontext* ctx,vmod* mod){
  ulist_vreloc* rells = mod->rells;
  ucursor cursor;
  rells->iterate(&cursor);
  while(1){
    vsymbol* symbol;
    vreloc* reloc = rells->next((uset*)rells,&cursor);
    if(!reloc){
      break;
    }
    symbol = vmod_symbol_get(ctx,mod,reloc->ref_name);
    if(!symbol){
      uabort1("relocate global symbol:%s not find!",reloc->ref_name->value);
    }
    vgc_array_set(reloc->rel_obj,reloc->rel_idx,symbol->slot);
  }
}

void vcontext_relocate(vcontext* ctx){
  ulist_vmod* mods = ctx->mods;
  ucursor cursor;
  mods->iterate(&cursor);
  while(1){
    vmod* mod = mods->next((uset*)mods,&cursor);
    if(!mod){
      break;
    }
    vmod_relocate(ctx,mod);
  }
}

vslot vcontext_params_get(vcontext* ctx,int index){
  vgc_call* calling;
  vgc_cfun* cfun;
  usize_t count;
  usize_t base;
  vslot slot;
  usize_t real_index;

  calling = vgc_obj_ref_get(ctx,calling,vgc_call);
  if(calling->call_type != vgc_call_type_cfun){
    uabort("vcontext_params_get:current call not a cfun!");
  }
  cfun = vgc_obj_ref_get(calling,cfun,vgc_cfun);
  count = cfun->params_count;
  base = calling->base;
  real_index = base - count + index;

  if(index < 0 || index >= count)
    uabort("vm:local varable error!");
  slot = vgc_heap_stack_get(ctx->heap,real_index);
  return slot;
}

void vcontext_mod_add(vcontext* ctx,vmod mod)
{
  if (ulist_vmod_append(ctx->mods,mod)) {
    uabort("vcontext add mod error!");
  }
}

void vmod_init(vmod* mod)
{
  ulist_vreloc* rells;
  uhstb_vsymbol* gobjtb;
  uhstb_vsymbol* lobjtb;
  
  rells = ulist_vreloc_new();
  if (!rells) {
    uabort("vmod rells new error!");
  }
  
  gobjtb = uhstb_vsymbol_new(VCONTEXT_OBJTB_SIZE);
  if (!gobjtb) {
    uabort("vmod gobjtb new error!");
  }

  lobjtb = uhstb_vsymbol_new(VCONTEXT_OBJTB_SIZE);
  if (!lobjtb) {
    uabort("vmod lobjtb new error!");
  }

  mod->rells = rells;
  mod->gobjtb = gobjtb;
  mod->lobjtb = lobjtb;
}

void vmod_add_reloc(vmod* mod,vreloc reloc)
{
  if (ulist_vreloc_append(mod->rells,reloc)){
    uabort("vmod add reloc error!");
  }
}

vsymbol* vmod_gobj_put(vmod* mod,ustring* name,vgc_obj* obj)
{
  vslot slot;
  vsymbol symbol;
  vsymbol* new_symbol;
  int retval;

  vslot_ref_set(slot,obj);
  symbol = (vsymbol){name,slot};
  retval = uhstb_vsymbol_put(mod->gobjtb,
				 name->hash_code,
				 &symbol,
				 &new_symbol,
				 NULL,
				 vobjtb_key_comp);
  if(retval == -1){
    uabort("vmod_gobj_put error!");
  }
  return new_symbol;
}

vsymbol* vmod_lobj_put(vmod* mod,ustring* name,vgc_obj* obj)
{
  vslot slot;
  vsymbol symbol;
  vsymbol* new_symbol;
  int retval;

  vslot_ref_set(slot,obj);
  symbol = (vsymbol){name,slot};
  retval = uhstb_vsymbol_put(mod->lobjtb,
				 name->hash_code,
				 &symbol,
				 &new_symbol,
				 NULL,
				 vobjtb_key_comp);
  if(retval == -1){
    uabort("vmod_lobj_put error!");
  }
  return new_symbol;  
}
