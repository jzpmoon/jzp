#ifndef _VPASS_H_
#define _VPASS_H_

#include "ulist_tpl.h"
#include "ugraph.h"
#include "vgc_obj.h"
#include "vreader.h"

#define VPSHEADER				\
  int t

enum vpsk{
  vpsk_dt,
  vpsk_mod,
  vpsk_inst,
  vpsk_type,
  vcfgk_blk,
  vcfgk_grp,
};

enum vdtk{
  vdtk_arr,
  vdtk_num,
  vdtk_int,
  vdtk_char,
  vdtk_str,
  vdtk_any,
  vdtk_code,
  vdtk_bool,
};

struct _vps_cntr;
typedef struct _vps_cntr vps_cntr;

typedef struct _vps_t{
  VPSHEADER;
} vps_t,* vpsp;

ulist_decl_tpl(vpsp);

#define vps_bool_true (1)
#define vps_bool_false (0)

typedef struct _vps_data{
  VPSHEADER;
  int dtk;
  int idx;
  int scope;
  ustring* name;
  union {
    double number;
    int integer;
    int character;
    int boolean;
    ustring* string;
    struct _vps_cfg* code;
  } u;
} vps_data,*vps_datap;

typedef struct _vps_id{
  ustring* name;
  int num;
} vps_id;

/*
 * instruction opcode kind
 */
enum viopck{
  viopck_branch,
  viopck_entry,
  viopck_non,
  viopck_ret,
};

/*
 * instruction operand kind
 */
enum viopek{
  vinstk_imm,
  vinstk_glodt,
  vinstk_locdt,
  vinstk_code,
  vinstk_non,
};

typedef struct _vps_opc{
  unsigned char iopck;
  usize_t opcode;
} vps_opc;

typedef struct _vps_ope{
  unsigned char iopek;
  usize_t operand;
  vps_id id;
  vps_data* data;
} vps_ope;

typedef struct _vps_inst{
  VPSHEADER;
  vps_opc opc;
  vps_ope ope[1];
} vps_inst,*vps_instp;

void vps_inst_log(vps_inst* inst);

#define vps_inst_opck_get(inst)			\
  (inst)->opc.iopck

#define vps_inst_opck_set(inst,k)		\
  (inst)->opc.iopck = k

#define vps_inst_opek_get(inst)			\
  (inst)->ope[0].iopek

#define vps_inst_opek_set(inst,k)		\
  (inst)->ope[0].iopek = k

#define vps_inst_isret(inst)			\
  ((inst)->opc.iopck == viopck_ret)

#define vps_inst_imm_set(inst,imm)		\
  (inst)->ope[0].operand = imm

#define vps_inst_imm_get(inst)			\
  (inst)->ope[0].operand

typedef struct _vps_type{
  VPSHEADER;
  ustring* type_name;
  int type_size;
  int type_index;
} vps_type,*vps_typep;

#define VCFGHEADER				\
  VPSHEADER;					\
  struct _vps_t* parent;			\
  vps_id id

vapi vps_id vcall
vps_id_get(vps_cntr* vps,ustring* name);

int vps_id_comp(vps_id id1,vps_id id2);

#define vps_id_log(id)					\
  do {							\
    if (id.name) {					\
      udebug2("id=%s:%d",id.name->value,id.num);	\
    } else {						\
      udebug1("id=:%d",id.num);				\
    }							\
  } while(0)

typedef struct _vps_cfg{
  VCFGHEADER;
} vps_cfg,*vps_cfgp;

uhstb_decl_tpl(vps_datap);
ulist_decl_tpl(vps_instp);
ulist_decl_tpl(vps_datap);
ulist_decl_tpl(vps_cfgp);

typedef struct _vcfg_block{
  VCFGHEADER;
  ugnode node;
  ulist_vps_instp* insts;
} vcfg_block;

#define VPS_SCOPE_UNKNOW -1
#define VPS_SCOPE_GLOBAL 1
#define VPS_SCOPE_LOCAL 2
#define VPS_SCOPE_ENTRY 3
#define VPS_SCOPE_DECL 4

typedef struct _vcfg_graph{
  VCFGHEADER;
  uhstb_vps_datap* locals;
  ulist_vps_instp* insts;
  ulist_vps_datap* imms;
  ulist_vps_cfgp* cfgs;
  vps_cfg* entry;
  int params_count;
  int locals_count;
  int scope;
} vcfg_graph,*vcfg_graphp;

uhstb_decl_tpl(vcfg_graphp);
uhstb_decl_tpl(vps_typep);

#define VPS_MOD_STATUS_LOADED 1
#define VPS_MOD_STATUS_UNLOAD 0

#define VPS_MOD_TYPE_ENTRY 1
#define VPS_MOD_TYPE_NORMAL 0

typedef struct _vps_mod{
  VPSHEADER;
  struct _vps_cntr* vps;
  uhstb_vps_datap* data;
  uhstb_vcfg_graphp* code;
  uhstb_vps_typep* types;
  vcfg_graph* entry;
  ustring* name;
  ustring* path;
  int status;
  int mod_type;
} vps_mod,*vps_modp;

uhstb_decl_tpl(vps_modp);

struct _vps_cntr
{
  umem_pool mp;
  uhstb_vps_modp* mods;
  vps_mod* entry;
  uhstb_vps_typep* types;
  int seqnum;
};

#define vps_cntr_alloc_get(vps) \
  &(vps)->mp.allocator

typedef struct _vps_jzp_req{
  VAST_ATTR_REQ_HEADER;
  vps_cntr* vps;
  vps_mod* top;
  vps_cfg* parent;
} vps_jzp_req;

#define VATTR_CONTEXT_FILE(parent)					\
  (parent->t == vcfgk_grp &&						\
   ((vcfg_graph*)parent)->scope == VPS_SCOPE_ENTRY)

#define VATTR_CONTEXT_SUBR(parent)			\
  (parent->t == vcfgk_grp &&				\
   ((vcfg_graph*)parent)->scope != VPS_SCOPE_ENTRY)

typedef vps_mod* (vcall *vps_prod_ft)
(vreader* reader,ustring* name,ustring* path,
				vps_cntr* vps);

UDECLFUN(UFNAME vfile2vps,
	 UARGS (vreader* reader,ustring* name,ustring* path,vps_cntr* vps),
	 URET vapi vps_mod* vcall)

vapi vps_inst* vcall
vps_inst_new(vps_cntr* vps,
	     int iopck,
	     int iopek,
	     usize_t opcode);

vapi vps_inst* vcall
vps_inop(vps_cntr* vps);

vapi vps_inst* vcall
vps_iloadimm(vps_cntr* vps,
		       int imm);

vapi vps_inst* vcall
vps_iloaddt(vps_cntr* vps,
		      ustring* name);

vapi vps_inst* vcall
vps_istoreimm(vps_cntr* vps,
			int imm);

vapi vps_inst* vcall
vps_istoredt(vps_cntr* vps,
		       ustring* name);

vapi vps_inst* vcall
vps_ipushint(vps_cntr* vps,
		       vcfg_graph* grp,
		       ustring* name,
		       int imm);

vapi vps_inst* vcall
vps_ipushchar(vps_cntr* vps,
			vcfg_graph* grp,
			int imm);

vapi vps_inst* vcall
vps_ipushnum(vps_cntr* vps,
		       vcfg_graph* grp,
		       ustring* name,
		       double dnum);

vapi vps_inst* vcall
vps_ipushdt(vps_cntr* vps,
		      vcfg_graph* graph,
		      ustring* name);

vapi vps_inst* vcall
vps_ipushstr(vps_cntr* vps,
		       vcfg_graph* graph,
		       ustring* string);

vapi vps_inst* vcall
vps_ipushnil(vps_cntr* vps,
		       vcfg_graph* grp);

vapi vps_inst* vcall
vps_ipushbool(vps_cntr* vps,
			vcfg_graph* grp,
			int bool);

vapi vps_inst* vcall
vps_itop(vps_cntr* vps,
		   int imm);

vapi vps_inst* vcall
vps_ipopdt(vps_cntr* vps,
		     vcfg_graph* graph,
		     ustring* name);

vapi vps_inst* vcall
vps_ipopv(vps_cntr* vps);

vapi vps_inst* vcall
vps_iadd(vps_cntr* vps);

vapi vps_inst* vcall
vps_isub(vps_cntr* vps);

vapi vps_inst* vcall
vps_imul(vps_cntr* vps);

vapi vps_inst* vcall
vps_idiv(vps_cntr* vps);

vapi vps_inst* vcall
vps_ijmpiimm(vps_cntr* vps,
		       int imm);

vapi vps_inst* vcall
vps_ijmpilb(vps_cntr* vps,
		      vps_id id);

vapi vps_inst* vcall
vps_ijmpimm(vps_cntr* vps,
		      int imm);

vapi vps_inst* vcall
vps_ijmplb(vps_cntr* vps,
		     vps_id id);

vapi vps_inst* vcall
vps_ieq(vps_cntr* vps);

vapi vps_inst* vcall
vps_igt(vps_cntr* vps);

vapi vps_inst* vcall
vps_ilt(vps_cntr* vps);

vapi vps_inst* vcall
vps_iand(vps_cntr* vps);

vapi vps_inst* vcall
vps_ior(vps_cntr* vps);

vapi vps_inst* vcall
vps_inot(vps_cntr* vps);

vapi vps_inst* vcall
vps_icall(vps_cntr* vps);

vapi vps_inst* vcall
vps_ireturn(vps_cntr* vps);

vapi vps_inst* vcall
vps_iretvoid(vps_cntr* vps);

vapi vps_inst* vcall
vps_irefimm(vps_cntr* vps,
		      int imm);

vapi vps_inst* vcall
vps_irefdt(vps_cntr* vps,
		     ustring* name);

vapi vps_inst* vcall
vps_isetimm(vps_cntr* vps,
		      int imm);

vapi vps_inst* vcall
vps_isetdt(vps_cntr* vps,
		     ustring* name);

vapi vps_inst* vcall
vps_ilabel(vps_cntr* vps,
		     vps_id id);

vapi vps_data* vcall
vps_num_new(vps_cntr* vps,
		      ustring* name,
		      double num);

vapi vps_data* vcall
vps_int_new(vps_cntr* vps,
		      ustring* name,
		      int inte);

vapi vps_data* vcall
vps_char_new(vps_cntr* vps,
		       int chara);

vapi vps_data* vcall
vps_str_new(vps_cntr* vps,
		      ustring* name,
		      ustring* string);

vapi vps_data* vcall
vps_any_new(vps_cntr* vps,
		      ustring* name);

vapi vps_data* vcall
vps_dtcd_new(vps_cntr* vps,
		       ustring* name,
		       vps_cfg* code);

vapi vps_data* vcall
vps_bool_new(vps_cntr* vps,
		       int bool);

vapi vcfg_block* vcall
vcfg_block_new(vps_cntr* vps,
			   vps_id id);

vapi void vcall
vcfg_blk_apd(vcfg_block* blk,
		  vps_inst* inst);

vps_inst*
vcfg_blk_linst_get(vcfg_block* blk);

vapi vcfg_graph* vcall
vcfg_graph_new(vps_cntr* vps,
			   ustring* name);

vapi void vcall
vcfg_grp_inst_apd(vcfg_graph* grp,
		       vps_inst* inst);

vps_inst* vcfg_grp_linst_get(vcfg_graph* grp);

#define vcfg_grp_inst_count(grp)		\
  ((grp)->insts->len)

#define vcfg_grp_inst_isempty(grp)		\
  (vcfg_grp_inst_count(grp) == 0)

UDECLFUN(UFNAME vcfg_grp_cdapd,
	 UARGS (vps_cntr* vps,vcfg_graph* grp,vps_cfg* cfg),
	 URET vapi void vcall);

UDECLFUN(UFNAME vcfg_grp_cdget,
	 UARGS (vps_cntr* vps,vcfg_graph* grp,vps_id id),
	 URET vapi vps_data* vcall);

UDECLFUN(UFNAME vcfg_grp_build,
	 UARGS (vps_cntr* vps,vcfg_graph* grp),
	 URET vapi void vcall);

UDECLFUN(UFNAME vcfg_grp_connect,
	 UARGS (vps_cntr* vps,vcfg_graph* grp),
	 URET vapi void vcall);

vapi void vcall
vcfg_grp_params_apd(vcfg_graph* grp,
			 vps_data* dt);

vapi void vcall
vcfg_grp_locals_apd(vcfg_graph* grp,
			 vps_data* dt);

vapi vps_data* vcall
vcfg_grp_dtget(vcfg_graph* grp,
			 ustring* name);

vapi int vcall
vcfg_grp_conts_put(vcfg_graph* grp,
		       vps_data* data);

vapi vps_mod* vcall
vps_mod_new(vps_cntr* vps,
		     ustring* name,
		     ustring* path);

#define vps_mod_loaded(mod)			\
  ((mod)->status = VPS_MOD_STATUS_LOADED)

#define vps_mod_isload(mod)			\
  ((mod)->status == VPS_MOD_STATUS_LOADED)

#define vps_mod_entry_set(mod)			\
  ((mod)->mod_type = VPS_MOD_TYPE_ENTRY)

#define vps_mod_normal_set(mod)			\
  ((mod)->mod_type = VPS_MOD_TYPE_NORMAL)

vapi void vcall
vps_mod_data_put(vps_mod* mod,
		      vps_data* data);

vapi void vcall
vps_mod_code_put(vps_mod* mod,
		      vcfg_graph* code);

UDECLFUN(UFNAME vps_mod_data_get,
	 UARGS (vps_mod* mod,
		ustring* name),
	 URET vapi vps_data* vcall);

UDECLFUN(UFNAME vps_mod_code_get,
	 UARGS (vps_mod* mod,
		ustring* name),
	 URET vapi vcfg_graph* vcall);

vapi void vcall
vps_cntr_init(vps_cntr* cntr);

vapi int vcall
vps_cntr_load(vps_cntr* vps,
		  vps_mod* mod);

UDECLFUN(UFNAME vps_cntr_data_get,
	 UARGS (vps_cntr* vps,ustring* name),
	 URET vapi vps_data* vcall);

UDECLFUN(UFNAME vps_cntr_code_get,
	 UARGS (vps_cntr* vps,ustring* name),
	 URET vapi vcfg_graph* vcall);

vapi void vcall
vps_cntr_clean(vps_cntr* vps);

vapi vps_type* vcall
vps_type_new(vps_cntr* vps,
		       ustring* type_name,
		       int type_size,
		       int type_index);

vapi void vcall
vps_ltype_put(vps_mod* mod,
		   vps_type* type);

vapi void vcall
vps_gtype_put(vps_cntr* vps,
		   vps_type* type);

vapi vps_type* vcall
vps_type_get(vps_mod* mod,
		       ustring* type_name);

#endif
