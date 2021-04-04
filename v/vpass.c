#include "uhstb_tpl.c"
#include "ulist_tpl.c"
#include "vbytecode.h"
#include "vm.h"
#include "vpass.h"

uhstb_def_tpl(vps_datap);
uhstb_def_tpl(vcfg_graphp);
uhstb_def_tpl(vps_modp);
uhstb_def_tpl(vps_typep);
ulist_def_tpl(vpsp);
ulist_def_tpl(vps_datap);
ulist_def_tpl(vps_cfgp);
ulist_def_tpl(vps_instp);

vps_inst*
vps_inst_new(vps_cntr* vps,
	     int instk,
	     usize_t opcode,
	     ustring* label,
	     vps_data* data,
	     vps_cfg* code){
  vps_inst* inst;

  inst = umem_pool_alloc(&vps->mp,sizeof(vps_inst));
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

vps_inst* vps_iloadimm(vps_cntr* vps,int imm){
  vps_inst* inst;
  inst = vps_inst_new(vps,vinstk_imm,Bload,NULL,NULL,NULL);
  if(inst){
    inst->inst.operand = imm;
  }
  return inst;
}

vps_inst* vps_iloaddt(vps_cntr* vps,ustring* name){
  vps_inst* inst;
  inst = vps_inst_new(vps,vinstk_locdt,Bload,name,NULL,NULL);
  return inst;
}

vps_inst* vps_istoreimm(vps_cntr* vps,int imm){
  vps_inst* inst;
  inst = vps_inst_new(vps,vinstk_imm,Bstore,NULL,NULL,NULL);
  if(inst){
    inst->inst.operand = imm;
  }
  return inst;  
}

vps_inst* vps_istoredt(vps_cntr* vps,ustring* name){
  vps_inst* inst;
  inst = vps_inst_new(vps,vinstk_locdt,Bstore,name,NULL,NULL);
  return inst;
}

vps_inst* vps_ipushint(vps_cntr* vps,
		       vcfg_graph* grp,
		       ustring* name,
		       int imm)
{
  vps_inst* inst;
  vps_data* data;

  data = vps_int_new(vps,name,imm);
  if (!data) {
    uabort("vps num new error!");
  }
  data->scope = VPS_SCOPE_LOCAL;
  data->idx = vps_graph_const_put(grp,data);
  inst = vps_inst_new(vps,vinstk_imm,Bpush,NULL,data,NULL);
  return inst;
}

vps_inst* vps_ipushnum(vps_cntr* vps,
		       vcfg_graph* grp,
		       ustring* name,
		       double dnum)
{
  vps_inst* inst;
  vps_data* data;

  data = vps_num_new(vps,name,dnum);
  if (!data) {
    uabort("vps num new error!");
  }
  data->scope = VPS_SCOPE_LOCAL;
  data->idx = vps_graph_const_put(grp,data);
  inst = vps_inst_new(vps,vinstk_imm,Bpush,NULL,data,NULL);
  return inst;
}

vps_inst* vps_ipushdt(vps_cntr* vps,vcfg_graph* grp,ustring* name)
{
  vps_inst* inst;
  vps_data* data;
  
  data = vps_any_new(vps,name,vstk_heap);
  if (!data) {
    uabort("vps num new error!");
  }
  data->scope = VPS_SCOPE_LOCAL;
  data->idx = vps_graph_const_put(grp,data);
  inst = vps_inst_new(vps,vinstk_glodt,Bpush,NULL,data,NULL);
  return inst;
}

vps_inst* vps_ipushstr(vps_cntr* vps,vcfg_graph* grp,ustring* string)
{
  vps_inst* inst;
  vps_data* data;
  
  data = vps_str_new(vps,string,string);
  if (!data) {
    uabort("vps num new error!");
  }
  data->scope = VPS_SCOPE_LOCAL;
  data->idx = vps_graph_const_put(grp,data);
  inst = vps_inst_new(vps,vinstk_imm,Bpush,NULL,data,NULL);
  return inst;
}

vps_inst* vps_itop(vps_cntr* vps,int imm){
  vps_inst* inst;
  inst = vps_inst_new(vps,vinstk_imm,Btop,NULL,NULL,NULL);
  if(inst){
    inst->inst.operand = imm;
  }
  return inst;
}

vps_inst* vps_ipopdt(vps_cntr* vps,vcfg_graph* grp,ustring* name)
{
  vps_inst* inst;
  vps_data* data;
  
  data = vps_any_new(vps,name,vstk_heap);
  if (!data) {
    uabort("vps num new error!");
  }
  data->scope = VPS_SCOPE_LOCAL;
  data->idx = vps_graph_const_put(grp,data);
  inst = vps_inst_new(vps,vinstk_glodt,Bpop,NULL,data,NULL);
  return inst;
}

vps_inst* vps_ipopv(vps_cntr* vps){
  vps_inst* inst;
  inst = vps_inst_new(vps,vinstk_non,Bpopv,NULL,NULL,NULL);
  return inst;
}

vps_inst* vps_iadd(vps_cntr* vps){
  vps_inst* inst;
  inst = vps_inst_new(vps,vinstk_non,Badd,NULL,NULL,NULL);
  return inst;
}

vps_inst* vps_isub(vps_cntr* vps){
  vps_inst* inst;
  inst = vps_inst_new(vps,vinstk_non,Bsub,NULL,NULL,NULL);
  return inst;
}

vps_inst* vps_imul(vps_cntr* vps){
  vps_inst* inst;
  inst = vps_inst_new(vps,vinstk_non,Bmul,NULL,NULL,NULL);
  return inst;
}

vps_inst* vps_idiv(vps_cntr* vps){
  vps_inst* inst;
  inst = vps_inst_new(vps,vinstk_non,Bdiv,NULL,NULL,NULL);
  return inst;
}

vps_inst* vps_ijmpiimm(vps_cntr* vps,int imm){
  vps_inst* inst;
  inst = vps_inst_new(vps,vinstk_imm,Bjmpi,NULL,NULL,NULL);
  if(inst){
    inst->inst.operand = imm;
  }
  return inst;  
}

vps_inst* vps_ijmpilb(vps_cntr* vps,ustring* label){
  vps_inst* inst;
  inst = vps_inst_new(vps,vinstk_code,Bjmpi,label,NULL,NULL);
  return inst;  
}

vps_inst* vps_ijmpimm(vps_cntr* vps,int imm){
  vps_inst* inst;
  inst = vps_inst_new(vps,vinstk_imm,Bjmp,NULL,NULL,NULL);
  if(inst){
    inst->inst.operand = imm;
  }
  return inst;  
}

vps_inst* vps_ijmplb(vps_cntr* vps,ustring* label){
  vps_inst* inst;
  inst = vps_inst_new(vps,vinstk_code,Bjmp,label,NULL,NULL);
  return inst;  
}

vps_inst* vps_ieq(vps_cntr* vps){
  vps_inst* inst;
  inst = vps_inst_new(vps,vinstk_non,Beq,NULL,NULL,NULL);
  return inst;
}

vps_inst* vps_igt(vps_cntr* vps){
  vps_inst* inst;
  inst = vps_inst_new(vps,vinstk_non,Bgt,NULL,NULL,NULL);
  return inst;
}

vps_inst* vps_ilt(vps_cntr* vps){
  vps_inst* inst;
  inst = vps_inst_new(vps,vinstk_non,Blt,NULL,NULL,NULL);
  return inst;
}

vps_inst* vps_iand(vps_cntr* vps){
  vps_inst* inst;
  inst = vps_inst_new(vps,vinstk_non,Band,NULL,NULL,NULL);
  return inst;
}

vps_inst* vps_ior(vps_cntr* vps){
  vps_inst* inst;
  inst = vps_inst_new(vps,vinstk_non,Bor,NULL,NULL,NULL);
  return inst;
}

vps_inst* vps_inot(vps_cntr* vps){
  vps_inst* inst;
  inst = vps_inst_new(vps,vinstk_non,Bnot,NULL,NULL,NULL);
  return inst;
}

vps_inst* vps_icall(vps_cntr* vps){
  vps_inst* inst;
  inst = vps_inst_new(vps,vinstk_non,Bcall,NULL,NULL,NULL);
  return inst;
}

vps_inst* vps_ireturn(vps_cntr* vps){
  vps_inst* inst;
  inst = vps_inst_new(vps,vinstk_non,Breturn,NULL,NULL,NULL);
  return inst;
}

vps_inst* vps_iretvoid(vps_cntr* vps){
  vps_inst* inst;
  inst = vps_inst_new(vps,vinstk_non,Bretvoid,NULL,NULL,NULL);
  return inst;
}

vps_inst* vps_inop(vps_cntr* vps){
  vps_inst* inst;
  inst = vps_inst_new(vps,vinstk_non,Bnop,NULL,NULL,NULL);
  return inst;  
}

vps_inst* vps_irefimm(vps_cntr* vps,int imm){
  vps_inst* inst;
  inst = vps_inst_new(vps,vinstk_imm,Bref,NULL,NULL,NULL);
  if(inst){
    inst->inst.operand = imm;
  }
  return inst;  
}

vps_inst* vps_irefdt(vps_cntr* vps,ustring* name)
{
  vps_inst* inst;
  inst = vps_inst_new(vps,vinstk_locdt,Brefl,name,NULL,NULL);
  return inst;
}

vps_inst* vps_isetimm(vps_cntr* vps,int imm){
  vps_inst* inst;
  inst = vps_inst_new(vps,vinstk_imm,Bset,NULL,NULL,NULL);
  if(inst){
    inst->inst.operand = imm;
  }
  return inst;  
}

vps_inst* vps_isetdt(vps_cntr* vps,ustring* name)
{
  vps_inst* inst;
  inst = vps_inst_new(vps,vinstk_locdt,Bsetl,name,NULL,NULL);
  return inst;
}

vps_data* vps_num_new(vps_cntr* vps,
		      ustring* name,
		      double num){
  vps_data* data;

  data = umem_pool_alloc(&vps->mp,sizeof(vps_data));
  if(data){
    data->t = vpsk_dt;
    data->dtk = vdtk_num;
    data->stk = vstk_heap;
    data->name = name;
    data->u.number = num;
  }
  return data;
}

vps_data* vps_int_new(vps_cntr* vps,
		      ustring* name,
		      int inte){
  vps_data* data;

  data = umem_pool_alloc(&vps->mp,sizeof(vps_data));
  if(data){
    data->t = vpsk_dt;
    data->dtk = vdtk_int;
    data->stk = vstk_heap;
    data->name = name;
    data->u.integer = inte;
  }
  return data;
}

vps_data* vps_str_new(vps_cntr* vps,
		      ustring* name,
		      ustring* string)
{
  vps_data* data;

  data = umem_pool_alloc(&vps->mp,sizeof(vps_data));
  if(data){
    data->t = vpsk_dt;
    data->dtk = vdtk_str;
    data->stk = vstk_heap;
    data->name = name;
    data->u.string = string;
  }
  return data;
}

vps_data* vps_any_new(vps_cntr* vps,
		      ustring* name,
		      int stk){
  vps_data* data;
  
  data = umem_pool_alloc(&vps->mp,sizeof(vps_data));
  if(data){
    data->t = vpsk_dt;
    data->dtk = vdtk_any;
    data->stk = stk;
    data->name = name;
  }
  return data;
}

vps_data* vps_dtcd_new(vps_cntr* vps,
		       ustring* name,
		       vps_cfg* code)
{
  vps_data* data;
  
  data = umem_pool_alloc(&vps->mp,sizeof(vps_data));
  if(data){
    data->t = vpsk_dt;
    data->dtk = vdtk_code;
    data->stk = vstk_stack;
    data->name = name;
    data->u.code = code;
  }
  return data;
}

vcfg_block* vcfg_block_new(vps_cntr* vps){
  vcfg_block* b;

  b = umem_pool_alloc(&vps->mp,sizeof(vcfg_block));
  if(b){
    b->t = vcfgk_blk;
    b->parent = NULL;
    b->name = NULL;
    b->insts = ulist_vps_instp_alloc(&vps->mp.allocator);
  }
  return b;
}

void vcfg_blk_apd(vcfg_block* blk,vps_inst* inst)
{
  ulist_vps_instp_append(blk->insts,inst);
}

vcfg_graph* vcfg_graph_new(vps_cntr* vps){
  vcfg_graph* g;
  uallocator* allocator;

  allocator = &vps->mp.allocator;
  g = umem_pool_alloc(&vps->mp,sizeof(vcfg_graph));
  if(g){
    g->t = vcfgk_grp;
    g->parent = NULL;
    g->name = NULL;
    g->locals = uhstb_vps_datap_alloc(allocator,VCFG_GRP_DATA_TABLE_SIZE);
    g->imms = ulist_vps_datap_alloc(allocator);
    g->cfgs = ulist_vps_cfgp_alloc(allocator);
    g->entry = NULL;
    g->params_count = 0;
    g->locals_count = 0;
  }
  return g;
}

static int vcfg_grp_dt_put_comp(vps_datap* data1,vps_datap* data2){
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

void vcfg_grp_cdapd(vps_cntr* vps,vcfg_graph* grp,vps_cfg* cfg)
{
  vps_data* data;
  ustring* name;
  int retval;

  name = cfg->name;
  if (name) {
    data = vps_dtcd_new(vps,name,cfg);
    if (!data) {
      uabort("vcfg_grp_cdapd: vps_dtcd_new error!");
    }
    retval = uhstb_vps_datap_put(grp->locals,
				 name->hash_code,
				 &data,
				 NULL,
				 NULL,
				 vcfg_grp_dt_put_comp);
    if (retval == 1) {
      uabort("vcfg_grp_cdapd: name:%s already exists!",name->value);
    } else if (retval == -1) {
      uabort("vcfg_grp_cdapd: locals put error!");      
    }
  }
  ulist_vps_cfgp_append(grp->cfgs,cfg);
}

void vcfg_grp_params_apd(vcfg_graph* grp,vps_data* dt){
  ustring* name;
  int retval;

  name = dt->name;
  if(!name){
    uabort("data name is null!");
  }
  retval = uhstb_vps_datap_put(grp->locals,
			       name->hash_code,
			       &dt,
			       NULL,
			       NULL,
			       vcfg_grp_dt_put_comp);
  if (retval == 1) {
    uabort("vcfg_grp_params_apd: name:%s already exists!",name->value);
  } else if (retval == -1) {
    uabort("vcfg_grp_params_apd: locals put error!");
  }
  grp->params_count++;
}

void vcfg_grp_locals_apd(vcfg_graph* grp,vps_data* dt){
  ustring* name;
  int retval;

  name = dt->name;
  if(!name){
    uabort("data name is null!");
  }
  retval = uhstb_vps_datap_put(grp->locals,
			       name->hash_code,
			       &dt,
			       NULL,
			       NULL,
			       vcfg_grp_dt_put_comp);
  if(retval == 1){
    uabort("vcfg_grp_locals_apd: name:%s already exists!",name->value);
  } else if(retval == -1){
    uabort("vcfg_grp_locals_apd: locals put error!");
  }
  grp->locals_count++;
}

static int vcfg_grp_dt_get_comp(vps_datap* data1,vps_datap* data2){
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

vps_data* vcfg_grp_dtget(vcfg_graph* grp,ustring* name){
  vps_data dt_in;
  vps_datap dt_ink = &dt_in;
  vps_datap* dt_outk;

  dt_in.name = name;
  uhstb_vps_datap_get(grp->locals,
		      name->hash_code,
		      &dt_ink,
		      &dt_outk,
		      vcfg_grp_dt_get_comp);
  if(dt_outk){
    return *dt_outk;
  }else{
    return NULL;
  }
}

vps_mod* vps_mod_new(vps_cntr* vps,ustring* name){
  vps_mod* mod;
  uallocator* allocator;

  allocator = &vps->mp.allocator;
  mod = umem_pool_alloc(&vps->mp,sizeof(vps_mod));
  if(mod){
    mod->t = vpsk_mod;
    mod->vps = vps;
    mod->data = uhstb_vps_datap_alloc(allocator,
				      VPS_MOD_DATA_TABLE_SIZE);
    if(!mod->data){
      uabort("new hash table data error!");
    }
    mod->code = uhstb_vcfg_graphp_alloc(allocator,
					VPS_MOD_CODE_TABLE_SIZE);
    if(!mod->code){
      uabort("new hash table code error!");
    }
    mod->types = uhstb_vps_typep_alloc(allocator,-1);
    if (!mod->types) {
      uabort("new hash table type error!");
    }
    mod->entry = NULL;
    mod->name = name;
    mod->status = VPS_MOD_STATUS_UNLOAD;
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

static int vps_mod_code_comp(vcfg_graphp* data1,vcfg_graphp* data2){
  vcfg_graph* g1 = *data1;
  vcfg_graph* g2 = *data2;
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

int vps_graph_const_put(vcfg_graph* grp,vps_data* data)
{
  int retval;
  
  retval = ulist_vps_datap_append(grp->imms,data);
  if (retval) {
    uabort("vps mod const put error!");
  }
  return grp->imms->len - 1;
}

void vps_mod_code_put(vps_mod* mod,vcfg_graph* code){
  ustring* name = code->name;
  unsigned int hscd = 0;
  if(name){
    hscd = name->hash_code;
  }
  if(uhstb_vcfg_graphp_put(mod->code,
			   hscd,
			   &code,
			   NULL,
			   NULL,
			   vps_mod_code_comp)){
    uabort("vps_mod_code_put error!");
  }
}

void vps_cntr_init(vps_cntr* cntr) {
  umem_pool_init(&cntr->mp,-1);
  cntr->mods = uhstb_vps_modp_alloc(&cntr->mp.allocator,
				    VPS_CNTR_MOD_TABLE_SIZE);
  if (!cntr->mods) {
    uabort("new hash table mod error!");
  }
  cntr->types = uhstb_vps_typep_alloc(&cntr->mp.allocator,-1);
  if (!cntr->types) {
    uabort("new hash table type error!");
  }
}

static int vps_cntr_mod_comp(vps_modp* mod1,vps_modp* mod2){
  ustring* name1 = (*mod1)->name;
  ustring* name2 = (*mod2)->name;
  return ustring_comp(name1,name2);
}

int vps_cntr_load(vps_cntr* vps,vps_mod* mod)
{
  ustring* name = mod->name;
  unsigned int hscd = name->hash_code;
  int retval;
  
  mod->vps = vps;
  retval = uhstb_vps_modp_put(vps->mods,
			      hscd,
			      &mod,
			      NULL,
			      NULL,
			      vps_cntr_mod_comp);
  if (retval == 1) {
    uabort("vps_cntr_load mod exists!");
  } else   if (retval == -1) {
    uabort("vps_cntr_load error!");
  }
  return 0;
}

void vps_cntr_clean(vps_cntr* vps)
{
  umem_pool_clean(&vps->mp);
}

static vps_type_comp(vps_typep* type1,vps_typep* type2)
{
  vps_type* t1;
  vps_type* t2;

  t1 = *type1;
  t2 = *type2;
  return ustring_comp(t1->type_name,t2->type_name);
}

vps_type* vps_type_new(vps_cntr* vps,
		       ustring* type_name,
		       int type_size,
		       int type_index)
{
  vps_type* type;
  uallocator* allocator;

  allocator = &vps->mp.allocator;
  type = allocator->alloc(allocator,sizeof(vps_type));
  if (type){
    type->t = vpsk_type;
    type->type_name = type_name;
    type->type_size = type_size;
    type->type_index = type_index;
  }

  return type;
}

void vps_ltype_put(vps_mod* mod,vps_type* type)
{
  ustring* type_name;
  int retval;

  type_name = type->type_name;
  retval = uhstb_vps_typep_put(mod->types,
			       type_name->hash_code,
			       &type,
			       NULL,
			       NULL,
			       vps_type_comp);
  if (retval == 1) {
    uabort("vps_type already exists!");
  } else if (retval == -1){
    uabort("vps_type put error!");
  }
}

void vps_gtype_put(vps_cntr* vps, vps_type* type)
{
  ustring* type_name;
  int retval;

  type_name = type->type_name;
  retval = uhstb_vps_typep_put(vps->types,
			       type_name->hash_code,
			       &type,
			       NULL,
			       NULL,
			       vps_type_comp);
  if (retval == 1) {
    uabort("vps_type already exists!");
  } else if (retval == -1){
    uabort("vps_type put error!");
  }
}

vps_type* vps_type_get(vps_mod* mod,ustring* type_name)
{
  vps_type type;
  vps_typep type_in;
  vps_typep* type_out;
  vps_cntr* vps;

  type.type_name = type_name;
  type_in = &type;
  uhstb_vps_typep_get(mod->types,
		      type_name->hash_code,
		      &type_in,
		      &type_out,
		      vps_type_comp);
  if (type_out) {
    return *type_out;
  }
  vps = mod->vps;
  uhstb_vps_typep_get(vps->types,
		      type_name->hash_code,
		      &type_in,
		      &type_out,
		      vps_type_comp);
  if (type_out) {
    return *type_out;
  } else {
    return NULL;
  }
}
