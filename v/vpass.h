#ifndef _VPASS_H_
#define _VPASS_H_

#include "ulist_tpl.h"
#include "vgc_obj.h"

#define VPSHEADER				\
  int t

enum{
  vpsk_dt,
  vpsk_mod,
  vpsk_inst,
  vdfgk_blk,
  vdfgk_grp,
};

enum{
  vinstk_imm,
  vinstk_glodt,
  vinstk_locdt,
  vinstk_code,
  vinstk_non,
};

enum{
  vdtk_arr,
  vdtk_num,
  vdtk_int,
  vdtk_str,
  vdtk_any,
  vdtk_code,
  vstk_heap,
  vstk_stack,
};

typedef struct _vps_t{
  VPSHEADER;
} vps_t,* vpsp;

ulist_decl_tpl(vpsp);

#define VPS_SCOPE_GLOBAL 1
#define VPS_SCOPE_LOCAL 2

typedef struct _vps_data{
  VPSHEADER;
  int dtk;
  int stk;
  int idx;
  int scope;
  ustring* name;
  union {
    double number;
    int integer;
    ustring* string;
    struct _vps_dfg* code;
  } u;
} vps_data,*vps_datap;

#define vopdnon (-1)

typedef struct _vinst{
  usize_t opcode;
  usize_t operand;
} vinst,*vinstp;

typedef struct _vps_inst{
  VPSHEADER;
  int instk;
  vinst inst;
  ustring* label;
  union {
    vps_data* data;
    struct _vps_dfg* code;
  } u;
} vps_inst,*vps_instp;

#define VDFGHEADER				\
  VPSHEADER;					\
  struct _vps_t* parent;			\
  ustring* name

typedef struct _vps_dfg{
  VDFGHEADER;
} vps_dfg,*vps_dfgp;

uhstb_decl_tpl(vps_datap);
ulist_decl_tpl(vps_datap);
ulist_decl_tpl(vps_instp);
ulist_decl_tpl(vps_dfgp);

typedef struct _vdfg_block{
  VDFGHEADER;
  ulist_vps_instp* insts;
} vdfg_block;

typedef struct _vdfg_graph{
  VDFGHEADER;
  uhstb_vps_datap* locals;
  ulist_vps_datap* imms;
  ulist_vps_dfgp* dfgs;
  vps_dfg* entry;
  int params_count;
  int locals_count;
  int scope;
} vdfg_graph,*vdfg_graphp;

#define VDFG_GRP_DATA_TABLE_SIZE 17

uhstb_decl_tpl(vdfg_graphp);

#define VPS_MOD_STATUS_LOADED 1
#define VPS_MOD_STATUS_UNLOAD 0

typedef struct _vps_mod{
  VPSHEADER;
  struct _vps_cntr* vps;
  uhstb_vps_datap* data;
  uhstb_vdfg_graphp* code;
  vdfg_graph* entry;
  ustring* name;
  int status;
} vps_mod,*vps_modp;

#define VPS_MOD_DATA_TABLE_SIZE 17
#define VPS_MOD_CODE_TABLE_SIZE 17

uhstb_decl_tpl(vps_modp);

typedef struct _vps_cntr{
  umem_pool mp;
  uhstb_vps_modp* mods;
} vps_cntr;

#define VPS_CNTR_MOD_TABLE_SIZE 17

ulist_decl_tpl(vinstp);

int vinst_to_str(vgc_heap* heap,
		 ulist_vinstp* insts);

vps_inst*
vps_inst_new(vps_cntr* vps,
	     int instk,
	     usize_t opcode,
	     ustring* label,
	     vps_data* data,
	     vps_dfg* code);

vps_inst* vps_inop(vps_cntr* vps);
vps_inst* vps_iloadimm(vps_cntr* vps,int imm);
vps_inst* vps_iloaddt(vps_cntr* vps,ustring* name);
vps_inst* vps_istoreimm(vps_cntr* vps,int imm);
vps_inst* vps_istoredt(vps_cntr* vps,ustring* name);
vps_inst* vps_ipushint(vps_cntr* vps,
		       vdfg_graph* grp,
		       ustring* name,
		       int imm);
vps_inst* vps_ipushnum(vps_cntr* vps,
		       vdfg_graph* grp,
		       ustring* name,
		       double dnum);
vps_inst* vps_ipushdt(vps_cntr* vps,vdfg_graph* graph,ustring* name);
vps_inst* vps_ipushstr(vps_cntr* vps,vdfg_graph* graph,ustring* string);
vps_inst* vps_itop(vps_cntr* vps,int imm);
vps_inst* vps_ipopdt(vps_cntr* vps,vdfg_graph* graph,ustring* name);
vps_inst* vps_ipopv(vps_cntr* vps);
vps_inst* vps_iadd(vps_cntr* vps);
vps_inst* vps_isub(vps_cntr* vps);
vps_inst* vps_imul(vps_cntr* vps);
vps_inst* vps_idiv(vps_cntr* vps);
vps_inst* vps_ijmpiimm(vps_cntr* vps,int imm);
vps_inst* vps_ijmpilb(vps_cntr* vps,ustring* label);
vps_inst* vps_ijmpimm(vps_cntr* vps,int imm);
vps_inst* vps_ijmplb(vps_cntr* vps,ustring* label);
vps_inst* vps_ieq(vps_cntr* vps);
vps_inst* vps_igt(vps_cntr* vps);
vps_inst* vps_ilt(vps_cntr* vps);
vps_inst* vps_iand(vps_cntr* vps);
vps_inst* vps_ior(vps_cntr* vps);
vps_inst* vps_inot(vps_cntr* vps);
vps_inst* vps_icall(vps_cntr* vps);
vps_inst* vps_ireturn(vps_cntr* vps);
vps_inst* vps_iretvoid(vps_cntr* vps);
vps_inst* vps_irefimm(vps_cntr* vps,int imm);
vps_inst* vps_isetimm(vps_cntr* vps,int imm);

vps_data* vps_num_new(vps_cntr* vps,
		      ustring* name,
		      double num);

vps_data* vps_int_new(vps_cntr* vps,
		      ustring* name,
		      int inte);

vps_data* vps_str_new(vps_cntr* vps,
		      ustring* name,
		      ustring* string);

vps_data* vps_any_new(vps_cntr* vps,
		      ustring* name,
		      int stk);

vps_data* vps_dtcd_new(vps_cntr* vps,
		       ustring* name,
		       vps_dfg* code);

vdfg_block* vdfg_block_new(vps_cntr* vps);

void vdfg_blk_apd(vdfg_block* blk,vps_inst* inst);

vdfg_graph* vdfg_graph_new(vps_cntr* vps);

void vdfg_grp_cdapd(vps_cntr* vps,vdfg_graph* grp,vps_dfg* dfg);

void vdfg_grp_params_apd(vdfg_graph* grp,vps_data* dt);

void vdfg_grp_locals_apd(vdfg_graph* grp,vps_data* dt);

vps_data* vdfg_grp_dtget(vdfg_graph* grp,ustring* name);

vps_mod* vps_mod_new(vps_cntr* vps,ustring* name);

#define vps_mod_loaded(mod)			\
  ((mod)->status = VPS_MOD_STATUS_LOADED)

#define vps_mod_isload(mod)			\
  ((mod)->status == VPS_MOD_STATUS_LOADED)

void vps_mod_data_put(vps_mod* mod,vps_data* data);

int vps_graph_const_put(vdfg_graph* grp,vps_data* data);

void vps_mod_code_put(vps_mod* mod,vdfg_graph* code);

void vps_cntr_init(vps_cntr* cntr);

int vps_cntr_load(vps_cntr* vps,vps_mod* mod);

void vps_cntr_clean(vps_cntr* vps);

#endif
