#ifndef _VPASS_H_
#define _VPASS_H_

#include "ulist_tpl.h"
#include "vgc_obj.h"
#include "vgenbc.h"

#define VPSHEADER				\
  int t

enum{
  vpsk_dt,
  vpsk_mod,
  vpsk_inst,
  vpsk_type,
  vcfgk_blk,
  vcfgk_grp,
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
  vdtk_char,
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
    int character;
    ustring* string;
    struct _vps_cfg* code;
  } u;
} vps_data,*vps_datap;

typedef struct _vps_inst{
  VPSHEADER;
  int instk;
  vinst inst;
  ustring* label;
  union {
    vps_data* data;
    struct _vps_cfg* code;
  } u;
} vps_inst,*vps_instp;

typedef struct _vps_type{
  VPSHEADER;
  ustring* type_name;
  int type_size;
  int type_index;
} vps_type,*vps_typep;

#define VCFGHEADER				\
  VPSHEADER;					\
  struct _vps_t* parent;			\
  ustring* name

typedef struct _vps_cfg{
  VCFGHEADER;
} vps_cfg,*vps_cfgp;

uhstb_decl_tpl(vps_datap);
ulist_decl_tpl(vps_instp);
ulist_decl_tpl(vps_datap);
ulist_decl_tpl(vps_cfgp);

typedef struct _vcfg_block{
  VCFGHEADER;
  ulist_vps_instp* insts;
} vcfg_block;

#define VPS_SCOPE_GLOBAL 1
#define VPS_SCOPE_LOCAL 2
#define VPS_SCOPE_ENTRY 3

typedef struct _vcfg_graph{
  VCFGHEADER;
  uhstb_vps_datap* locals;
  ulist_vps_datap* imms;
  ulist_vps_cfgp* cfgs;
  vps_cfg* entry;
  int params_count;
  int locals_count;
  int scope;
} vcfg_graph,*vcfg_graphp;

#define VCFG_GRP_DATA_TABLE_SIZE 17

uhstb_decl_tpl(vcfg_graphp);
uhstb_decl_tpl(vps_typep);

#define VPS_MOD_STATUS_LOADED 1
#define VPS_MOD_STATUS_UNLOAD 0

typedef struct _vps_mod{
  VPSHEADER;
  struct _vps_cntr* vps;
  uhstb_vps_datap* data;
  uhstb_vcfg_graphp* code;
  uhstb_vps_typep* types;
  vcfg_graph* entry;
  ustring* name;
  int status;
} vps_mod,*vps_modp;

#define VPS_MOD_DATA_TABLE_SIZE 17
#define VPS_MOD_CODE_TABLE_SIZE 17

uhstb_decl_tpl(vps_modp);

typedef struct _vps_cntr{
  umem_pool mp;
  uhstb_vps_modp* mods;
  uhstb_vps_typep* types;
} vps_cntr;

#define VPS_CNTR_MOD_TABLE_SIZE 17

vps_inst*
vps_inst_new(vps_cntr* vps,
	     int instk,
	     usize_t opcode,
	     ustring* label,
	     vps_data* data,
	     vps_cfg* code);

vps_inst* vps_inop(vps_cntr* vps);
vps_inst* vps_iloadimm(vps_cntr* vps,int imm);
vps_inst* vps_iloaddt(vps_cntr* vps,ustring* name);
vps_inst* vps_istoreimm(vps_cntr* vps,int imm);
vps_inst* vps_istoredt(vps_cntr* vps,ustring* name);
vps_inst* vps_ipushint(vps_cntr* vps,
		       vcfg_graph* grp,
		       ustring* name,
		       int imm);
vps_inst* vps_ipushchar(vps_cntr* vps,
			vcfg_graph* grp,
			int imm);
vps_inst* vps_ipushnum(vps_cntr* vps,
		       vcfg_graph* grp,
		       ustring* name,
		       double dnum);
vps_inst* vps_ipushdt(vps_cntr* vps,vcfg_graph* graph,ustring* name);
vps_inst* vps_ipushstr(vps_cntr* vps,vcfg_graph* graph,ustring* string);
vps_inst* vps_ipushnil(vps_cntr* vps,vcfg_graph* grp);
vps_inst* vps_itop(vps_cntr* vps,int imm);
vps_inst* vps_ipopdt(vps_cntr* vps,vcfg_graph* graph,ustring* name);
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
vps_inst* vps_irefdt(vps_cntr* vps,ustring* name);
vps_inst* vps_isetimm(vps_cntr* vps,int imm);
vps_inst* vps_isetdt(vps_cntr* vps,ustring* name);

vps_data* vps_num_new(vps_cntr* vps,
		      ustring* name,
		      double num);

vps_data* vps_int_new(vps_cntr* vps,
		      ustring* name,
		      int inte);

vps_data* vps_char_new(vps_cntr* vps,
		       int chara);

vps_data* vps_str_new(vps_cntr* vps,
		      ustring* name,
		      ustring* string);

vps_data* vps_any_new(vps_cntr* vps,
		      ustring* name,
		      int stk);

vps_data* vps_dtcd_new(vps_cntr* vps,
		       ustring* name,
		       vps_cfg* code);

vcfg_block* vcfg_block_new(vps_cntr* vps);

void vcfg_blk_apd(vcfg_block* blk,vps_inst* inst);

vcfg_graph* vcfg_graph_new(vps_cntr* vps);

void vcfg_grp_cdapd(vps_cntr* vps,vcfg_graph* grp,vps_cfg* cfg);

void vcfg_grp_params_apd(vcfg_graph* grp,vps_data* dt);

void vcfg_grp_locals_apd(vcfg_graph* grp,vps_data* dt);

vps_data* vcfg_grp_dtget(vcfg_graph* grp,ustring* name);

vps_mod* vps_mod_new(vps_cntr* vps,ustring* name);

#define vps_mod_loaded(mod)			\
  ((mod)->status = VPS_MOD_STATUS_LOADED)

#define vps_mod_isload(mod)			\
  ((mod)->status == VPS_MOD_STATUS_LOADED)

void vps_mod_data_put(vps_mod* mod,vps_data* data);

int vps_graph_const_put(vcfg_graph* grp,vps_data* data);

void vps_mod_code_put(vps_mod* mod,vcfg_graph* code);

void vps_cntr_init(vps_cntr* cntr);

int vps_cntr_load(vps_cntr* vps,vps_mod* mod);

void vps_cntr_clean(vps_cntr* vps);

vps_type* vps_type_new(vps_cntr* vps,
		       ustring* type_name,
		       int type_size,
		       int type_index);

void vps_ltype_put(vps_mod* mod,vps_type* type);

void vps_gtype_put(vps_cntr* vps, vps_type* type);

vps_type* vps_type_get(vps_mod* mod,ustring* type_name);

#endif
