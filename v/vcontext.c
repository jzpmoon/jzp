#include "uhstb_tpl.c"
#include "ulist_tpl.c"
#include "vcontext.h"

ulist_def_tpl(vreloc);
uhstb_def_tpl(vmod);
uhstb_def_tpl(vsymbol);

static void gdata_load(vcontext* ctx,vmod* mod,vps_data* data);
static void ldata_load(vgc_heap* heap,vgc_array* consts,vps_data* data);
static vslot data2data(vgc_heap* heap,vps_data* data);

UDECLFUN(UFNAME insts_concat,
	 UARGS (vgc_array* consts,
		ulist_vps_cfgp* cfgs,
		vcfg_block* blk,
		vmod* mod,
		ulist_vps_instp* insts),
	 URET static int);

UDEFUN(UFNAME vcontext_new,
       UARGS (vgc_heap* heap),
       URET vapi vcontext* vcall)
UDECLARE
  vcontext* ctx;
  uhstb_vmod* mods;
  ustring_table* symtb;
  ustring_table* strtb;
UBEGIN
  ctx = vgc_heap_obj_new(heap,
			 vcontext,
			 vgc_obj_type_ctx,
			 vgc_heap_area_static);

  if(!ctx){
    uabort("vcontext_new: ctx new error!");
  }

  umem_pool_init(&ctx->mp,-1);
  
  mods = uhstb_vmod_new(-1);
  if (!mods) {
    uabort("vcontext_new:mods new error!");
  }
    
  symtb = ustring_table_new(-1);
  if(!symtb){
    uabort("vcontext_new:symtb new error!");
  }

  strtb = ustring_table_new(-1);
  if(!strtb){
    uabort("vcontext_new:strtb new error!");
  }

  ctx->heap = heap;
  ctx->stack = &heap->root_set;
  ctx->mods = mods;
  ctx->loader = NULL;
  ctx->symtb = symtb;
  ctx->strtb = strtb;
  vgc_obj_null_set(ctx,calling);

  return ctx;
UEND

vgc_subr* vgc_subr_new(vcontext* ctx,
		       usize_t params_count,
		       usize_t locals_count,
		       int area_type){
  vgc_subr* subr;
  subr = vgc_heap_obj_new(ctx->heap,
			  vgc_subr,
			  vgc_obj_type_subr,
			  area_type);
  if(subr){
    subr->params_count = params_count;
    subr->locals_count = locals_count;
    vcontext_obj_slot_set(ctx,subr,consts);
    vcontext_obj_slot_set(ctx,subr,bytecode);
  }
  return subr;
}

vapi vgc_cfun* vcall
vgc_cfun_new(vgc_heap* heap,
             vcfun_ft entry,
             int params_count,
             int has_retval,
             int area_type)
{
  vgc_cfun* cfun;
  if(params_count < 0){
    return NULL;
  }
  cfun = (vgc_cfun*)vgc_heap_data_new(heap,
				      sizeof(vgc_cfun),
				      0,
				      vgc_obj_type_cfun,
				      area_type);
  if(cfun){
    cfun->entry = entry;
    cfun->params_count = params_count;
    cfun->has_retval = has_retval;
  }
  return cfun;
}

vgc_call* vgc_call_new(vcontext* ctx,
		       int call_type,
		       usize_t base){
  vgc_heap* heap;
  vgc_call* call;
  vgc_subr* subr;
  vgc_string* bytecode;

  heap = ctx->heap;
  call = vgc_heap_obj_new(heap,
			  vgc_call,
			  vgc_obj_type_call,
			  vgc_heap_area_active);
  if(call){
    call->call_type = call_type;
    call->base = base;
    if(call_type == vgc_call_type_cfun){
      vcontext_obj_slot_set(ctx,call,cfun);
      vgc_obj_null_set(call,subr);
    }else{
      vcontext_obj_pop(ctx,subr,vgc_subr);
      vcontext_obj_slot_get(ctx,subr,bytecode);
      vcontext_obj_pop(ctx,bytecode,vgc_string);
      call->pc = bytecode->u.b;
      vcontext_obj_push(ctx,subr);
      vcontext_obj_slot_set(ctx,call,subr);
      vgc_obj_null_set(call,cfun);
    }
    vcontext_obj_slot_set(ctx,call,caller);
  }
  return call;
}

static int ucall vobjtb_key_comp(vsymbol* sym1,vsymbol* sym2){
  return ustring_comp(sym1->name, sym2->name);
}

static int get_insts_count(ulist_vps_cfgp* cfgs,vps_id id)
{
  ucursor cursor;
  int find_flag = 0;
  int insts_count = 0;

  cfgs->iterate(&cursor);
  while(1){
    vps_cfgp* cfgp = cfgs->next((uset*)cfgs,&cursor);
    vps_cfg* cfg; 
    vcfg_block* blk;
    if(!cfgp){
      break;
    }
    cfg = *cfgp;
    if(cfg->t != vcfgk_blk){
      uabort("vps_cfg not a block!");
    }
    blk = (vcfg_block*)cfg;
    if (!vps_id_comp(blk->id,id)) {
      find_flag = 1;
      break;
    }
    insts_count += blk->insts->len;
  }
  if (!find_flag) {
    uabort("can not find label!");
  }
  return insts_count;
}

UDEFUN(UFNAME insts_concat,
       UARGS (vgc_array* consts,
	      ulist_vps_cfgp* cfgs,
	      vcfg_block* blk,
	      vmod* mod,
	      ulist_vps_instp* insts),
       URET static int)
UDECLARE
  /*part insts*/
  ulist_vps_instp* pinsts;
  ucursor cursor;
UBEGIN
  pinsts = blk->insts;
  insts->iterate(&cursor);

  while(1){
    vps_instp* instp = insts->next((uset*)pinsts,&cursor);
    vps_inst* pinst;

    if (!instp) {
      break;
    }
    pinst = *instp;
#define DF(code,name,value,len,oct)			\
    case code:					\
      if (len > 1) {				\
	goto l1;				\
      }else {					\
	goto l2;				\
      }						\
      break;
    switch (pinst->opc.opcode) {
      VBYTECODE
#undef DF
    }
  l1:
    switch (vps_inst_opek_get(pinst)) {
    case vinstk_imm:
      if(pinst->ope[0].data){
	vps_inst_imm_set(pinst,pinst->ope[0].data->idx);
      }
      ulist_vps_instp_append(insts,pinst);
      udebug0("inst imm");
      break;
    case vinstk_locdt:
      {
	vps_t* cfg = blk->parent;
	vcfg_graph* grp;
	vps_data* data;
	
	if(!cfg || cfg->t != vcfgk_grp){
	  uabort("vcfg_block have no parent!");
	}
	grp = (vcfg_graph*)cfg;
	data = vcfg_grp_dtget(grp,pinst->ope[0].id.name);
	if(!data){
	  vps_id_log(pinst->ope[0].id);
	  uabort("local variable: not find");
	}
	vps_inst_imm_set(pinst,data->idx);
	ulist_vps_instp_append(insts,pinst);
	udebug0("inst local data");
      }
      break;
    case vinstk_glodt:
      {
	vps_data* data = pinst->ope[0].data;
	vreloc reloc;

	reloc.ref_name = data->name;
	reloc.rel_idx = data->idx;
	reloc.rel_obj = consts;
	vmod_add_reloc(mod,reloc);
	vps_inst_imm_set(pinst,data->idx);
	ulist_vps_instp_append(insts,pinst);
	udebug0("inst global data");
      }
      break;
    case vinstk_code:
      {
	int insts_count;

	insts_count = get_insts_count(cfgs,pinst->ope[0].id);
	vps_inst_imm_set(pinst,insts_count);
	ulist_vps_instp_append(insts,pinst);
	udebug0("inst code");
      }
      break;
    case vinstk_non:
      {
	ulist_vps_instp_append(insts,pinst);
	udebug0("inst non");
      }
      break;
    default:
      break;
    }
    continue;
  l2:
    ulist_vps_instp_append(insts,pinst);
    udebug0("inst non operand");
  }
  return 0;
UEND

static void ldata_load(vgc_heap* heap,vgc_array* consts,vps_data* data)
{
  vslot slot;
  int top;

  slot = data2data(heap,data);
  top = vgc_array_push(consts,slot);
  if(top == -1){
    uabort("vcontext_load consts overflow!");
  }
  data->idx = top;
}

static vslot data2data(vgc_heap* heap,vps_data* data)
{
  vslot slot;

  if(data->dtk == vdtk_num){
    vslot_num_set(slot,data->u.number);      
  }else if(data->dtk == vdtk_int){
    vslot_int_set(slot,data->u.integer);
  }else if(data->dtk == vdtk_str){
    vgc_string* vstr;
    ustring* ustr;
    
    ustr = data->u.string;
    vstr = vgc_string_new(heap,
			  ustr->len,
			  vgc_heap_area_static);
    if (!vstr) {
      uabort("new vgc_string error!");
    }
    vgc_ustr2vstr(vstr,ustr);
    vslot_ref_set(slot,vstr);
  }else if(data->dtk == vdtk_char){
    vslot_char_set(slot,data->u.character);
  }else if(data->dtk == vdtk_bool){
    vslot_bool_set(slot,data->u.boolean);
  }else if(data->dtk == vdtk_any){
    vslot_null_set(slot);
  }else{
    uabort("vcontext_load unknow data type");
    vslot_null_set(slot);
  }
  return slot;
}

UDEFUN(UFNAME vcontext_graph_load,
       UARGS (vcontext* ctx,vmod* mod,vcfg_graph* grp),
       URET vgc_subr*)
UDECLARE
  vgc_heap* heap;
  vgc_array* consts;
  uallocator* allocator;
  ulist_vps_datap* imms;
  ulist_vps_cfgp* cfgs;
  ulist_vps_instp* insts;
  vgc_string* bytecode;
  vgc_subr* subr;
  ucursor cursor;
UBEGIN
  heap = ctx->heap;
  consts = vgc_array_new(heap,
			 grp->imms->len,
			 vgc_heap_area_static);
  if (!consts) {
    uabort("subr consts new error!");
  }
  /* load code */
  cfgs = grp->cfgs;

  allocator = vcontext_alloc_get(ctx);
  insts = ulist_vps_instp_alloc(allocator);
  
  cfgs->iterate(&cursor);
  while (1) {
    vps_cfgp* cfgp = cfgs->next((uset*)cfgs,&cursor);
    vcfg_block* blk;
    if (!cfgp) {
      break;
    }
    if ((*cfgp)->t != vcfgk_blk) {
      uabort("vps_cfg not a block!");
    }
    blk = (vcfg_block*)(*cfgp);
    if (insts_concat(consts,cfgs,blk,mod,insts)) {
      uabort("insts concat error!");
    }
  }

  bytecode = vps_inst_to_str(heap,insts);
  if (!bytecode) {
    uabort("vps inst to str error!");
  }
  vcontext_obj_push(ctx,bytecode);
  vcontext_obj_push(ctx,consts);
  subr = vgc_subr_new(ctx,
		      grp->params_count,
		      grp->locals_count,
		      vgc_heap_area_static);
  if (!subr) {
    uabort("new subr error!");
  }
  if (grp->scope == VPS_SCOPE_UNKNOW) {
    uabort("graph scope unknow!");
  }
  if (grp->scope != VPS_SCOPE_ENTRY) {
    if (!grp->id.name) {
      uabort("graph not a entry but has no name!");
    }
    /*
     * add to local symbol table
     */
    vmod_lobj_put(heap,mod,grp->id.name,(vgc_obj*)subr);
    /*
     * if scope is global then also add to global symbol table
     */
    if (grp->scope == VPS_SCOPE_GLOBAL) {
      vmod_gobj_put(heap,mod,grp->id.name,(vgc_obj*)subr);
    }
  }
  allocator->clean(allocator);
  /* load imm data */
  imms = grp->imms;
  imms->iterate(&cursor);
  while(1){
    vps_datap* datap = imms->next((uset*)imms,&cursor);
    vps_data* data;
    if (!datap) {
      break;
    }
    data = *datap;
    ldata_load(heap,consts,data);
  }

  return subr;
UEND

void vcontext_mod_log(vcontext* ctx){
  uhstb_vmod* mods;
  ucursor cursor;
  
  mods = ctx->mods;
  mods->iterate(&cursor);
  while (1) {
    vmod* next = mods->next((uset*)mods,&cursor);
    if (!next) {
      break;
    }
    udebug1("vcontext_mod_log:%s",next->name->value);
  }
}

UDEFUN(UFNAME vcontext_mod2mod,
       UARGS (vcontext* ctx,vmod* dest_mod,vps_mod* src_mod),
       URET vapi int vcall)
UDECLARE
  ucursor cursor;
  uhstb_vps_datap* data = NULL;
  uhstb_vcfg_graphp* code = NULL;
UBEGIN
  data = src_mod->data;
  code = src_mod->code;

  if (vps_mod_isload(src_mod)) {
    vmod_loaded(dest_mod);
  }

  (data->iterate)(&cursor);
  while(1){
    vps_datap* dp;
    vps_data* d;
    dp = (data->next)((uset*)data,&cursor);
    if(!dp){
      break;
    }
    d = *dp;
    gdata_load(ctx,dest_mod,d);
  }

  (code->iterate)(&cursor);
  while(1){
    vcfg_graphp* gp;
    vcfg_graph* g;
    gp = (code->next)((uset*)code,&cursor);
    if(!gp){
      break;
    }
    g = *gp;
    udebug0("load vps graph name:");
    vps_id_log(g->id);
    vcontext_graph_load(ctx,dest_mod,g);
  }

  if (src_mod->entry) {
    udebug0("load vps entry graph");
    vps_id_log(src_mod->entry->id);
    dest_mod->init = vcontext_graph_load(ctx,dest_mod,src_mod->entry);
  }
  return 0;
UEND

UDEFUN(UFNAME vcontext_mod_load,
       UARGS (vcontext* ctx,vps_mod* mod),
       URET vapi int vcall)
UDECLARE
  vmod* dest_mod;
UBEGIN
  dest_mod = vcontext_mod_add(ctx,mod->name,mod->path);
  vcontext_mod2mod(ctx,dest_mod,mod);
  return 0;
UEND

static void vcontext_mod_init(vcontext* ctx)
{
  uhstb_vmod* mods;
  ucursor cursor;
  
  mods = ctx->mods;
  mods->iterate(&cursor);
  while (1) {
    vmod* next = mods->next((uset*)mods,&cursor);
    vslot slot;
    if (!next) {
      break;
    }
    if (next->init) {
      vslot_ref_set(slot,next->init);
      vcontext_stack_push(ctx,slot);
      vcontext_execute(ctx);
    }
  }
}
UDEFUN(UFNAME vcontext_vps_load,
       UARGS (vcontext* ctx,vps_cntr* vps),
       URET vapi int vcall)
UDECLARE
  ucursor cursor;
  uhstb_vps_modp* mods;
UBEGIN
  mods = vps->mods;
  mods->iterate(&cursor);
  while (1) {
    vps_modp* next = mods->next((uset*)mods,&cursor);
    vps_mod* mod;
    if (!next) {
      break;
    }
    mod = *next;
    udebug1("load vps mod:%s",mod->name->value);
    vcontext_mod_load(ctx,mod);
  }
  vps_cntr_clean(vps);
  vcontext_relocate(ctx);
  vcontext_mod_init(ctx);
  return 0;
UEND

static void gdata_load(vcontext* ctx,vmod* mod,vps_data* data)
{
  vslot slot;

  slot = data2data(ctx->heap,data);
  vmod_lslot_put(ctx->heap,mod,data->name,slot);
  if (data->scope == VPS_SCOPE_GLOBAL) {
    vmod_gslot_put(ctx->heap,mod,data->name,slot);
  }
}

vsymbol* vmod_symbol_get(vcontext* ctx,vmod* mod,ustring* name){
  vsymbol symbol_in;
  vsymbol* symbol_out;
  uhstb_vmod* mods;
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
    if (!vmod_isload(next) && ctx->loader) {
      ctx->loader->load(ctx->loader,next);
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
    vreloc* reloc;
    reloc = rells->next((uset*)rells,&cursor);
    if(!reloc){
      break;
    }
    symbol = vmod_symbol_get(ctx,mod,reloc->ref_name);
    if(!symbol){
      uabort("relocate global symbol:%s not find!",reloc->ref_name->value);
    }
    vgc_array_set(reloc->rel_obj,reloc->rel_idx,symbol->slot);
  }
}

void vcontext_relocate(vcontext* ctx){
  uhstb_vmod* mods = ctx->mods;
  ucursor cursor;

  mods->iterate(&cursor);
  while(1){
    vmod* mod = mods->next((uset*)mods,&cursor);
    if(!mod){
      break;
    }
    if (vmod_isload(mod)) {
      vmod_relocate(ctx,mod);
    } else if (ctx->loader) {
      ctx->loader->load(ctx->loader,mod);
      vmod_relocate(ctx,mod);
    }
  }
}

vapi vslot vcall vcontext_params_get(vcontext* ctx,int index){
  vgc_call* calling;
  vgc_cfun* cfun;
  int count;
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
  slot = vcontext_stack_get(ctx,real_index);
  return slot;
}

static int ucall vcontext_mod_comp(vmod* mod1,vmod* mod2){
  return ustring_comp(mod1->name,mod2->name);
}

static void vmod_init(vmod* mod,ustring* name,ustring* path)
{
  ulist_vreloc* rells;
  uhstb_vsymbol* gobjtb;
  uhstb_vsymbol* lobjtb;
  
  rells = ulist_vreloc_new();
  if (!rells) {
    uabort("vmod rells new error!");
  }
  
  gobjtb = uhstb_vsymbol_new(-1);
  if (!gobjtb) {
    uabort("vmod gobjtb new error!");
  }

  lobjtb = uhstb_vsymbol_new(-1);
  if (!lobjtb) {
    uabort("vmod lobjtb new error!");
  }

  mod->rells = rells;
  mod->gobjtb = gobjtb;
  mod->lobjtb = lobjtb;
  mod->init = NULL;
  mod->name = name;
  mod->path = path;
  mod->status = VMOD_STATUS_UNLOAD;
}

vapi vmod* vcall vcontext_mod_add(vcontext* ctx,
                                  ustring* name,
                                  ustring* path)
{
  vmod in_mod;
  vmod* out_mod;
  int retval;

  vmod_init(&in_mod,name,path);
  retval = uhstb_vmod_put(ctx->mods,
			  name->hash_code,
			  &in_mod,
			  &out_mod,
			  NULL,
			  vcontext_mod_comp);
  if (retval == 1) {
    uabort("vcontext add mod exists!");
  } else if (retval == -1) {
    uabort("vcontext add mod error!");
  }
  return out_mod;
}

void vmod_add_reloc(vmod* mod,vreloc reloc)
{
  if (ulist_vreloc_append(mod->rells,reloc)){
    uabort("vmod add reloc error!");
  }
}

vapi vsymbol* vcall
vmod_gobj_put(vgc_heap* heap,vmod* mod,ustring* name,vgc_obj* obj)
{
  vslot slot;
  vsymbol* symbol;

  vslot_ref_set(slot,obj);
  symbol = vmod_gslot_put(heap,mod,name,slot);
  
  return symbol;
}

vapi vsymbol* vcall
vmod_lobj_put(vgc_heap* heap,vmod* mod,ustring* name,vgc_obj* obj)
{
  vslot slot;
  vsymbol* symbol;

  vslot_ref_set(slot,obj);
  symbol = vmod_lslot_put(heap,mod,name,slot);
  
  return symbol;
}

vsymbol* vmod_gslot_put(vgc_heap* heap,vmod* mod,ustring* name,vslot slot)
{
  vgc_ref* ref;
  vslot sym_slot;
  vsymbol symbol;
  vsymbol* new_symbol;
  int retval;

  ref = vgc_ref_new(heap,slot);
  if (!ref) {
    uabort("ref new error!");
  }
  vslot_ref_set(sym_slot,ref);
  symbol.name = name;
  symbol.slot = sym_slot;
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

vsymbol* vmod_lslot_put(vgc_heap* heap,vmod* mod,ustring* name,vslot slot)
{
  vgc_ref* ref;
  vslot sym_slot;
  vsymbol symbol;
  vsymbol* new_symbol;
  int retval;

  ref = vgc_ref_new(heap,slot);
  if (!ref) {
    uabort("ref new error!");
  }
  vslot_ref_set(sym_slot,ref);
  symbol.name = name;
  symbol.slot = sym_slot;
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

vslot vcontext_stack_get(vcontext* ctx,usize_t index){
  vslot slot;
  if(ustack_vslot_get(ctx->stack,index,&slot)){
    uabort("vcontext_stack:index over of bound!");
  }
  return slot;
}

void vcontext_stack_set(vcontext* ctx,usize_t index,vslot slot){
  if(ustack_vslot_set(ctx->stack,index,slot)){
    uabort("vcontext_stack:index over of bound!");
  }
}

usize_t vcontext_stack_top_get(vcontext* ctx){
  return ustack_vslot_top_get(ctx->stack);
}

void vcontext_stack_top_set(vcontext* ctx,usize_t index){
  if(ustack_vslot_top_set(ctx->stack,index)){
    uabort("vcontext_stack:index over of bound!");
  }
}

vapi void vcall
vcontext_stack_push(vcontext* ctx, vslot slot)
{
    if (ustack_vslot_push(ctx->stack, slot)) {
        uabort("vgc_heap_stack: overflow!");
    }
}

vapi void vcall
vcontext_stack_pop(vcontext* ctx, vslot* slotp)
{
    if (ustack_vslot_pop(ctx->stack, slotp)) {
        uabort("vgc_heap_stack: empty!");
    }
}

vapi void vcall
vcontext_stack_pushv(vcontext* ctx)
{
    if (ustack_vslot_pushv(ctx->stack)) {
        uabort("vgc_heap_stack: overflow!");
    }
}
