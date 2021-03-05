#include "uhstb_tpl.c"
#include "ulist_tpl.c"
#include "vcontext.h"

ulist_def_tpl(vreloc);
uhstb_def_tpl(vmod);
uhstb_def_tpl(vsymbol);

static void gdata_load(vcontext* ctx,vmod* mod,vps_data* data);
static void ldata_load(vgc_heap* heap,vgc_array* consts,vps_data* data);
static vslot data2data(vgc_heap* heap,vps_data* data);
static int inst2inst(vgc_array* consts,
		     ulist_vps_dfgp* dfgs,
		     vdfg_block* blk,
		     vmod* mod,
		     ulist_vinstp* insts);

#define VCONTEXT_MODTB_SIZE 17
#define VCONTEXT_OBJTB_SIZE 17
#define VCONTEXT_SYMTB_SIZE 17
#define VCONTEXT_STRTB_SIZE 17

UDEFUN(UFNAME vcontext_new,UARGS (vgc_heap* heap),URET vcontext*,
UDECLARE
  vcontext* ctx;
  uhstb_vmod* mods;
  ustring_table* symtb;
  ustring_table* strtb;
)
UBEGIN
({
  ctx = vgc_heap_obj_new(heap,
			 vcontext,
			 vgc_obj_type_ctx,
			 vgc_heap_area_static);

  if(!ctx){
    uabort("vcontext_new: ctx new error!");
  }

  umem_pool_init(&ctx->mp,-1);
  
  mods = uhstb_vmod_new(VCONTEXT_MODTB_SIZE);
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

  ctx->heap = heap;
  ctx->mods = mods;
  ctx->loader = NULL;
  ctx->symtb = symtb;
  ctx->strtb = strtb;
  vgc_obj_null_set(ctx,calling);

  return ctx;
})

vgc_cfun* vgc_cfun_new(vgc_heap* heap,
		       vcfun_ft entry,
		       int params_count,
		       int has_retval,
		       int area_type){
  vgc_cfun* cfun;
  if(params_count < 0){
    return NULL;
  }
  cfun = vgc_heap_obj_new(heap,
			  vgc_cfun,
			  vgc_obj_type_cfun,
			  area_type);
  if(cfun){
    cfun->entry = entry;
    cfun->params_count = params_count;
    cfun->has_retval = has_retval;
  }
  return cfun;
}

static int vobjtb_key_comp(vsymbol* sym1,vsymbol* sym2){
  return ustring_comp(sym1->name, sym2->name);
}

static int get_insts_count(ulist_vps_dfgp* dfgs,ustring* name)
{
  ucursor cursor;
  int find_flag = 0;
  int insts_count = 0;
  
  dfgs->iterate(&cursor);
  while(1){
    vps_dfgp* dfgp = dfgs->next((uset*)dfgs,&cursor);
    vps_dfg* dfg; 
    vdfg_block* blk;
    if(!dfgp){
      break;
    }
    dfg = *dfgp;
    if(dfg->t != vdfgk_blk){
      uabort("vps_dfg not a block!");
    }
    blk = (vdfg_block*)dfg;
    if (blk->name == name) {
      find_flag = 1;
      break;
    }
    insts_count += blk->insts->len;
  }
  if (!find_flag) {
    uabort("can not find label:%s!",name->value);
  }
  return insts_count;
}

static int inst2inst(vgc_array* consts,
		     ulist_vps_dfgp* dfgs,
		     vdfg_block* blk,
		     vmod* mod,
		     ulist_vinstp* insts)
{
  ulist_vps_instp* src_insts;
  ucursor cursor;

  src_insts = blk->insts;
  insts->iterate(&cursor);

  while(1){
    vps_instp* instp = insts->next((uset*)src_insts,&cursor);
    vps_inst* src_inst;
    vinst* inst;

    if(!instp){
      break;
    }
    src_inst = *instp;
    switch(src_inst->instk){
    case vinstk_imm:
      inst = &src_inst->inst;
      if(src_inst->u.data){
	inst->operand = src_inst->u.data->idx;
      }
      ulist_vinstp_append(insts,inst);
      ulog("inst imm");
      break;
    case vinstk_locdt:
      {
	vps_t* dfg = blk->parent;
	vdfg_graph* grp;
	vps_data* data;
	
	if(!dfg || dfg->t != vdfgk_grp){
	  uabort("vdfg_block have no parent!");
	}
	grp = (vdfg_graph*)dfg;
	data = vdfg_grp_dtget(grp,src_inst->label);
	if(!data){
	  uabort("local variable: %s not find",src_inst->label->value);
	}
	inst = &src_inst->inst;
	inst->operand = data->idx;
	ulist_vinstp_append(insts,inst);
	ulog("inst local data");
      }
      break;
    case vinstk_glodt:
      {
	vps_data* data = src_inst->u.data;
	vreloc reloc;

	reloc.ref_name = data->name;
	reloc.rel_idx = data->idx;
	reloc.rel_obj = consts;
	vmod_add_reloc(mod,reloc);
	inst = &src_inst->inst;
	inst->operand = data->idx;
	ulist_vinstp_append(insts,inst);
	ulog("inst global data");      
      }
      break;
    case vinstk_code:
      {
	int insts_count;

	inst = &src_inst->inst;
	insts_count = get_insts_count(dfgs,src_inst->label);
	inst->operand = insts_count;
	ulist_vinstp_append(insts,inst);
	ulog("inst code");
      }
      break;
    case vinstk_non:
      {
	inst = &src_inst->inst;
	ulist_vinstp_append(insts,inst);
	ulog("inst non");
      }
      break;
    default:
      break;
    }
  }
  return 0;
}

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
  ulog("vcontext_load data:%s,idx:%d",data->name->value,top);
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
  }else if(data->dtk == vdtk_arr){
    /* todo */
    vslot_null_set(slot);
  }else if(data->dtk == vdtk_any){
    vslot_null_set(slot);
  }else{
    uabort("vcontext_load unknow data type");
    vslot_null_set(slot);
  }
  return slot;
}

vgc_subr* vcontext_graph_load(vcontext* ctx,vmod* mod,vdfg_graph* grp){
  vgc_heap* heap;
  vgc_array* consts;
  ulist_vps_datap* imms;
  ulist_vps_dfgp* dfgs;
  ulist_vinstp* insts;
  vgc_subr* subr;
  ucursor cursor;

  heap = ctx->heap;
  consts = vgc_array_new(heap,
			 grp->imms->len,
			 vgc_heap_area_static);
  if(!consts){
    uabort("subr consts new error!");
  }
  /* load code */
  dfgs = grp->dfgs;
  insts = ulist_vinstp_alloc(&ctx->mp.allocator);
  
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
    if(inst2inst(consts,dfgs,blk,mod,insts)){
      uabort("inst2inst error!");
    }
  }

  vinst_to_str(heap,insts);
  vgc_heap_obj_push(heap,consts);
  subr = vgc_subr_new(heap,
		      grp->params_count,
		      grp->locals_count,
		      vgc_heap_area_static);
  if (!subr) {
    uabort("new subr error!");
  }
  /*
   * add to local symbol table
   */
  vmod_lobj_put(heap,mod,grp->name,(vgc_obj*)subr);
  /*
   * if scope is global then also add to global symbol table
   */
  if (grp->scope == VPS_SCOPE_GLOBAL) {
    vmod_gobj_put(heap,mod,grp->name,(vgc_obj*)subr);
  }
  umem_pool_clean(&ctx->mp);
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
}

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
    ulog("vcontext_mod_log:%s",next->name->value);
  }
}

int vcontext_mod2mod(vcontext* ctx,vmod* dest_mod,vps_mod* src_mod)
{
  ucursor cursor;
  uhstb_vps_datap* data = src_mod->data;
  uhstb_vdfg_graphp* code = src_mod->code;
    
  if (vps_mod_isload(src_mod)) {
    vmod_loaded(dest_mod);
  }

  (data->iterate)(&cursor);
  while(1){
    vps_datap* dp = (data->next)((uset*)data,&cursor);
    vps_data* d;
    if(!dp){
      break;
    }
    d = *dp;
    gdata_load(ctx,dest_mod,d);
  }

  (code->iterate)(&cursor);
  while(1){
    vdfg_graphp* gp = (code->next)((uset*)code,&cursor);
    vdfg_graph* g;
    if(!gp){
      break;
    }
    g = *gp;
    vcontext_graph_load(ctx,dest_mod,g);
  }
  if (src_mod->entry) {
    dest_mod->init = vcontext_graph_load(ctx,dest_mod,src_mod->entry);
  }
  return 0;
}

int vcontext_mod_load(vcontext* ctx,vps_mod* mod)
{
  vmod* dest_mod;

  dest_mod = vcontext_mod_add(ctx,mod->name);
  vcontext_mod2mod(ctx,dest_mod,mod);
  return 0;
}

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
      vgc_heap_stack_push(ctx->heap,slot);
      vcontext_execute(ctx);
    }
  }
}

int vcontext_vps_load(vcontext* ctx,vps_cntr* vps)
{
  ucursor cursor;
  uhstb_vps_modp* mods;

  mods = vps->mods;
  mods->iterate(&cursor);
  while (1) {
    vps_modp* next = mods->next((uset*)mods,&cursor);
    vps_mod* mod;
    if (!next) {
      break;
    }
    mod = *next;
    vcontext_mod_load(ctx,mod);
  }
  vps_cntr_clean(vps);
  vcontext_relocate(ctx);
  vcontext_mod_init(ctx);
  return 0;
}

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

static int vcontext_mod_comp(vmod* mod1,vmod* mod2){
  return ustring_comp(mod1->name,mod2->name);
}

void vmod_init(vmod* mod,ustring* name)
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
  mod->init = NULL;
  mod->name = name;
  mod->status = VMOD_STATUS_UNLOAD;
}

vmod* vcontext_mod_add(vcontext* ctx,ustring* name)
{
  vmod in_mod;
  vmod* out_mod;
  int retval;

  vmod_init(&in_mod,name);
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

vsymbol* vmod_gobj_put(vgc_heap* heap,vmod* mod,ustring* name,vgc_obj* obj)
{
  vslot slot;
  vsymbol* symbol;

  vslot_ref_set(slot,obj);
  symbol = vmod_gslot_put(heap,mod,name,slot);
  
  return symbol;
}

vsymbol* vmod_lobj_put(vgc_heap* heap,vmod* mod,ustring* name,vgc_obj* obj)
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
