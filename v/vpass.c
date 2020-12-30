#include "uhstb_tpl.c"
#include "ulist_tpl.c"
#include "vbytecode.h"
#include "vm.h"
#include "vpass.h"

uhstb_def_tpl(vps_datap);
uhstb_def_tpl(vdfg_graphp);
uhstb_def_tpl(vps_mod);
ulist_def_tpl(vpsp);
ulist_def_tpl(vps_instp);
ulist_def_tpl(vps_dfgp);
ulist_def_tpl(vinstp);

vps_inst*
vps_inst_new(int instk,
	     usize_t opcode,
	     ustring* label,
	     vps_data* data,
	     vps_dfg* code){
  vps_inst* inst = ualloc(sizeof(vps_inst));
  if(inst){
    inst->t = vpsk_inst;
    inst->instk = instk;
    inst->inst.opcode = opcode;
    inst->label = label;
    if(data){
      inst->u.data = data;
    }else{
      inst->u.code = code;
    }
  }
  return inst;
}

vps_inst* vps_iloadimm(int imm){
  vps_inst* inst;
  inst = vps_inst_new(vinstk_imm,Bload,NULL,NULL,NULL);
  if(inst){
    inst->inst.operand = imm;
  }
  return inst;
}

vps_inst* vps_iloaddt(ustring* name){
  vps_inst* inst;
  inst = vps_inst_new(vinstk_locdt,Bload,name,NULL,NULL);
  return inst;
}

vps_inst* vps_istoreimm(int imm){
  vps_inst* inst;
  inst = vps_inst_new(vinstk_imm,Bstore,NULL,NULL,NULL);
  if(inst){
    inst->inst.operand = imm;
  }
  return inst;  
}

vps_inst* vps_istoredt(ustring* name){
  vps_inst* inst;
  inst = vps_inst_new(vinstk_locdt,Bstore,name,NULL,NULL);
  return inst;
}

vps_inst* vps_ipushimm(vps_mod* mod,ustring* name,double dnum){
  vps_inst* inst;
  vps_data* data;
  data = vps_num_new(name,dnum,vstk_heap);
  vps_mod_data_put(mod,data);
  inst = vps_inst_new(vinstk_imm,Bpush,NULL,data,NULL);
  return inst;
}

vps_inst* vps_ipushdt(vps_mod* mod,ustring* name){
  vps_inst* inst;
  vps_data* data;
  data = vps_any_new(name,vstk_heap);
  vps_mod_data_put(mod,data);
  inst = vps_inst_new(vinstk_glodt,Bpush,NULL,data,NULL);
  return inst;
}

vps_inst* vps_itop(int imm){
  vps_inst* inst;
  inst = vps_inst_new(vinstk_imm,Btop,NULL,NULL,NULL);
  ulog1("vps_itop:%d",imm);
  if(inst){
    inst->inst.operand = imm;
  }
  return inst;
}

vps_inst* vps_ipop(){
  vps_inst* inst;
  inst = vps_inst_new(vinstk_non,Bpop,NULL,NULL,NULL);
  return inst;
}

vps_inst* vps_iadd(){
  vps_inst* inst;
  inst = vps_inst_new(vinstk_non,Badd,NULL,NULL,NULL);
  return inst;
}

vps_inst* vps_isub(){
  vps_inst* inst;
  inst = vps_inst_new(vinstk_non,Bsub,NULL,NULL,NULL);
  return inst;
}

vps_inst* vps_imul(){
  vps_inst* inst;
  inst = vps_inst_new(vinstk_non,Bmul,NULL,NULL,NULL);
  return inst;
}

vps_inst* vps_idiv(){
  vps_inst* inst;
  inst = vps_inst_new(vinstk_non,Bdiv,NULL,NULL,NULL);
  return inst;
}

vps_inst* vps_ijmpiimm(int imm){
  vps_inst* inst;
  inst = vps_inst_new(vinstk_imm,Bjmpi,NULL,NULL,NULL);
  if(inst){
    inst->inst.operand = imm;
  }
  return inst;  
}

vps_inst* vps_ijmpimm(int imm){
  vps_inst* inst;
  inst = vps_inst_new(vinstk_imm,Bjmp,NULL,NULL,NULL);
  if(inst){
    inst->inst.operand = imm;
  }
  return inst;  
}

vps_inst* vps_ieq(){
  vps_inst* inst;
  inst = vps_inst_new(vinstk_non,Beq,NULL,NULL,NULL);
  return inst;
}

vps_inst* vps_igt(){
  vps_inst* inst;
  inst = vps_inst_new(vinstk_non,Bgt,NULL,NULL,NULL);
  return inst;
}

vps_inst* vps_ilt(){
  vps_inst* inst;
  inst = vps_inst_new(vinstk_non,Blt,NULL,NULL,NULL);
  return inst;
}

vps_inst* vps_iand(){
  vps_inst* inst;
  inst = vps_inst_new(vinstk_non,Band,NULL,NULL,NULL);
  return inst;
}

vps_inst* vps_ior(){
  vps_inst* inst;
  inst = vps_inst_new(vinstk_non,Bor,NULL,NULL,NULL);
  return inst;
}

vps_inst* vps_inot(){
  vps_inst* inst;
  inst = vps_inst_new(vinstk_non,Bnot,NULL,NULL,NULL);
  return inst;
}

vps_inst* vps_icall(){
  vps_inst* inst;
  inst = vps_inst_new(vinstk_non,Bcall,NULL,NULL,NULL);
  return inst;
}

vps_inst* vps_ireturn(){
  vps_inst* inst;
  inst = vps_inst_new(vinstk_non,Breturn,NULL,NULL,NULL);
  return inst;
}

vps_inst* vps_iretvoid(){
  vps_inst* inst;
  inst = vps_inst_new(vinstk_non,Bretvoid,NULL,NULL,NULL);
  return inst;
}

vps_inst* vps_inop(){
  vps_inst* inst;
  inst = vps_inst_new(vinstk_non,Bnop,NULL,NULL,NULL);
  return inst;  
}

vps_inst* vps_irefimm(int imm){
  vps_inst* inst;
  inst = vps_inst_new(vinstk_imm,Bref,NULL,NULL,NULL);
  if(inst){
    inst->inst.operand = imm;
  }
  return inst;  
}

vps_inst* vps_isetimm(int imm){
  vps_inst* inst;
  inst = vps_inst_new(vinstk_imm,Bset,NULL,NULL,NULL);
  if(inst){
    inst->inst.operand = imm;
  }
  return inst;  
}

usize_t
vinst_full_length(ulist_vinstp* insts){
  ucursor cursor;
  usize_t length = 0;
  insts->iterate(&cursor);
  while(1){
    vinstp* inst = insts->next((uset*)insts,&cursor);
    if(!inst){
      break;
    }
    switch((*inst)->opcode){
#define DF(code,name,value,len)			\
      case (code):length+=len;break;
      VBYTECODE
#undef DF
    }
  };
  return length;
}

usize_t
vinst_byte_length(ulist_vinstp* insts,usize_t offset){
  usize_t i      = 0;
  usize_t length = 0;
  ucursor cursor;
  insts->iterate(&cursor);
  while(i < offset){
    vinstp* inst = insts->next((uset*)insts,&cursor);
    if(!inst){
      break;
    }
    switch((*inst)->opcode){
#define DF(code,name,value,len)			\
      case (code):length+=len;break;
      VBYTECODE
#undef DF
    }
    i++;
  }
  return length;
}

int vinst_to_str(vcontext* ctx,ulist_vinstp* insts){
  ucursor cursor;
  usize_t inst_count = 0;
  usize_t byte_count = 0;
  usize_t length     = vinst_full_length(insts);
  vgc_string* str;
  str = vgc_string_new(ctx->heap,
		       length,
		       vgc_heap_area_active);
  if(!str){
    uabort("vinst_to_str:vgc_string new error!");
  }
  insts->iterate(&cursor);
  while(1){
    vinstp* instp = insts->next((uset*)insts,&cursor);
    vinst* inst;
    if(!instp){
      break;
    }
    inst = *instp;
    switch(inst->opcode){
#define DF(ocode,name,value,len)					\
      case (ocode):{							\
	int i = 0;							\
	usize_t operand;						\
	if(ocode == Bjmp || ocode == Bjmpi){				\
	  operand = vinst_byte_length(insts,				\
				      inst_count + inst->operand);	\
	}else{								\
	  operand = inst->operand;					\
	}								\
	str->u.b[byte_count++] = inst->opcode;				\
	while(i<len-1&&i<sizeof(usize_t)){				\
	  str->u.b[byte_count++] = operand>>(8*i);			\
	  i++;								\
	}								\
	inst_count++;							\
	break;								\
      }						
      VBYTECODE				
#undef DF
	}
  };
  vgc_heap_obj_push(ctx->heap,str);
  return 0;
}

vps_data* vps_num_new(ustring* name,
		      double num,
		      int stk){
  vps_data* data = ualloc(sizeof(vps_data));
  if(data){
    data->t = vpsk_dt;
    data->dtk = vdtk_num;
    data->stk = stk;
    data->name = name;
    data->u.number = num;
  }
  return data;
}

vps_data* vps_any_new(ustring* name,
		      int stk){
  vps_data* data = ualloc(sizeof(vps_data));
  if(data){
    data->t = vpsk_dt;
    data->dtk = vdtk_any;
    data->name = name;
  }
  return data;
}

vdfg_block* vdfg_block_new(){
  vdfg_block* b = ualloc(sizeof(vdfg_block));
  if(b){
    b->t = vdfgk_blk;
    b->parent = NULL;
    b->insts = ulist_vps_instp_new();
  }
  return b;
}

int vdfg_blk2inst(vcontext* ctx,vdfg_block* blk,ulist_vinstp* insts){
  ulist_vps_instp* insts_l1;
  ucursor cursor;
  insts_l1 = blk->insts;
  insts->iterate(&cursor);

  while(1){
    vps_instp* instp = insts->next((uset*)insts_l1,&cursor);
    vps_inst* inst_l1;
    vinst* inst;

    if(!instp){
      break;
    }
    inst_l1 = *instp;
    switch(inst_l1->instk){
    case vinstk_imm:
      inst = &inst_l1->inst;
      if(inst_l1->u.data){
	inst->operand = inst_l1->u.data->idx;
      }
      ulist_vinstp_append(insts,inst);
      ulog("inst imm");
      break;
    case vinstk_locdt:{
      vps_t* dfg = blk->parent;
      vdfg_graph* grp;
      vps_data* data;

      if(!dfg || dfg->t != vdfgk_grp){
	uabort("vdfg_block have no parent!");
      }
      grp = (vdfg_graph*)dfg;
      data = vdfg_grp_dtget(grp,inst_l1->label);
      if(!data){
	uabort1("local variable: %s not find",inst_l1->label->value);
      }
      inst = &inst_l1->inst;
      inst->operand = data->idx;
      ulist_vinstp_append(insts,inst);
      ulog("inst local data");
    }
      break;
    case vinstk_glodt:{
      ulist_vreloc* rells = ctx->rells;
      vps_data* data = inst_l1->u.data;
      vgc_array* consts;
      vreloc reloc;
      consts = vgc_obj_ref_get(ctx,consts,vgc_array);
      reloc.ref_name = data->name;
      reloc.rel_idx = data->idx;
      reloc.rel_obj = consts;
      ulist_vreloc_append(rells,reloc);
      inst = &inst_l1->inst;
      inst->operand = data->idx;
      ulist_vinstp_append(insts,inst);
      ulog("inst global data");      
    }
      break;
    case vinstk_code:
      ulog("inst code");
      break;
    case vinstk_non:
      inst = &inst_l1->inst;
      ulist_vinstp_append(insts,inst);
      ulog("inst non");
      break;
    default:
      break;
    }
  }
  return 0;
}

vdfg_graph* vdfg_graph_new(){
  vdfg_graph* g = ualloc(sizeof(vdfg_graph));
  if(g){
    g->t = vdfgk_grp;
    g->parent = NULL;
    g->name = NULL;
    g->locals = uhstb_vps_datap_new(VDFG_GRP_DATA_TABLE_SIZE);
    g->dfgs = ulist_vps_dfgp_new();
    g->entry = NULL;
    g->params_count = 0;
    g->locals_count = 0;
  }
  return g;
}

static int vdfg_grp_dt_put_comp(vps_datap* data1,vps_datap* data2){
  vps_data* d1;
  vps_data* d2;
  ustring* n1;
  ustring* n2;
  d1 = *data1;
  d2 = *data2;
  n1 = d1->name;
  n2 = d2->name;
  return ustring_comp(n1,n2);
}

void vdfg_grp_params_apd(vdfg_graph* grp,vps_data* dt){
  ustring* name = dt->name;
  unsigned int hscd = 0;
  if(name){
    hscd = name->hash_code;
  }
  if(uhstb_vps_datap_put(grp->locals,
			 hscd,
			 &dt,
			 NULL,
			 NULL,
			 vdfg_grp_dt_put_comp)){
    uabort("vdfg_grp_params_apd error!");
  }
  grp->params_count++;
}

void vdfg_grp_locals_apd(vdfg_graph* grp,vps_data* dt){
  ustring* name = dt->name;
  unsigned int hscd = 0;
  if(name){
    hscd = name->hash_code;
  }
  if(uhstb_vps_datap_put(grp->locals,
			 hscd,
			 &dt,
			 NULL,
			 NULL,
			 vdfg_grp_dt_put_comp)){
    uabort("vdfg_grp_locals_apd error!");
  }
  grp->locals_count++;
}

static int vdfg_grp_dt_get_comp(vps_datap* data1,vps_datap* data2){
  vps_data* d1;
  vps_data* d2;
  ustring* n1;
  ustring* n2;
  d1 = *data1;
  d2 = *data2;
  n1 = d1->name;
  n2 = d2->name;
  return ustring_comp(n1,n2);
}

vps_data* vdfg_grp_dtget(vdfg_graph* grp,ustring* name){
  vps_data dt_in;
  vps_datap dt_ink = &dt_in;
  vps_datap* dt_outk;

  dt_in.name = name;
  uhstb_vps_datap_get(grp->locals,
		      name->hash_code,
		      &dt_ink,
		      &dt_outk,
		      vdfg_grp_dt_get_comp);
  if(dt_outk){
    return *dt_outk;
  }else{
    return NULL;
  }
}

vps_mod* vps_mod_new(){
  vps_mod* mod = ualloc(sizeof(vps_mod));
  if(mod){
    mod->t = vpsk_mod;
    mod->data = uhstb_vps_datap_new(VPS_MOD_DATA_TABLE_SIZE);
    if(!mod->data){
      uabort("new hash table data error!");
    }
    mod->code = uhstb_vdfg_graphp_new(VPS_MOD_CODE_TABLE_SIZE);
    if(!mod->code){
      uabort("new hash table code error!");
    }
    mod->entry = NULL;
  }
  return mod;
}

static int vps_mod_data_comp(vps_datap* data1,vps_datap* data2){
  vps_data* d1 = *data1;
  vps_data* d2 = *data2;
  if(d1 > d2){
    return 1;
  }else if(d1 < d2){
    return -1;
  }else{
    return 0;
  }
}

static int vps_mod_code_comp(vdfg_graphp* data1,vdfg_graphp* data2){
  vdfg_graph* g1 = *data1;
  vdfg_graph* g2 = *data2;
  if(g1 > g2){
    return 1;
  }else if(g1 < g2){
    return -1;
  }else{
    return 0;
  }
}

void vps_mod_data_put(vps_mod* mod,vps_data* data){
  ustring* name = data->name;
  unsigned int hscd = 0;
  if(name){
    hscd = name->hash_code;
  }
  if(uhstb_vps_datap_put(mod->data,
			 hscd,
			 &data,
			 NULL,
			 NULL,
			 vps_mod_data_comp)){
    uabort("vps_mod_data_put error!");
  }
}

void vps_mod_code_put(vps_mod* mod,vdfg_graph* code){
  ustring* name = code->name;
  unsigned int hscd = 0;
  if(name){
    hscd = name->hash_code;
  }
  if(uhstb_vdfg_graphp_put(mod->code,
			   hscd,
			   &code,
			   NULL,
			   NULL,
			   vps_mod_code_comp)){
    uabort("vps_mod_code_put error!");
  }
}

void vps_cntr_init(vps_cntr* cntr) {
  umem_pool_init(&cntr->pool);
  cntr->mods = uhstb_vps_mod_newmp(&cntr->pool,VPS_CNTR_MOD_TABLE_SIZE);
}

static int vps_cntr_mod_comp(vps_mod* mod1,vps_mod* mod2){
  ustring* name1 = mod1->name;
  ustring* name2 = mod2->name;
  return ustring_comp(name1,name2);
}

int vps_cntr_load(vps_cntr* vps,vps_mod* mod){
  ustring* name = mod->name;
  unsigned int hscd = name->hash_code;
  
  if (uhstb_vps_mod_put(vps->mods,
			hscd,
			mod,
			NULL,
			NULL,
			vps_cntr_mod_comp)) {
    uabort("vps_cntr_load error!");
  }
  return 0;
}
