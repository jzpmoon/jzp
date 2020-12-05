#ifndef _VPASS_H_
#define _VPASS_H_

#include "ulist_tpl.h"
#include "vgc_obj.h"

#define VPSHEADER				\
  int t

#define VDFGHEADER				\
  VPSHEADER;					\
  struct _vps_t* parent

enum{
  vpsk_dt,
  vpsk_mod,
  vpsk_inst,
  vdfgk_blk,
  vdfgk_grp,
};

enum{
  vinstk_imm,
  vinstk_data,
  vinstk_code,
  vinstk_non,
};

enum{
  vdtk_arr,
  vdtk_num,
  vdtk_int,
  vdtk_any,
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
  ustring* name;
  union {
    ulist_vpsp* array;
    double number;
    int integer;
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

typedef struct _vps_dfg{
  VDFGHEADER;
} vps_dfg,*vps_dfgp;

ulist_decl_tpl(vps_instp);
ulist_decl_tpl(vps_datap);
ulist_decl_tpl(vps_dfgp);

typedef struct _vdfg_block{
  VDFGHEADER;
  ulist_vps_instp* insts;
} vdfg_block;

typedef struct _vdfg_graph{
  VDFGHEADER;
  ustring* name;
  ulist_vps_datap* params;
  ulist_vps_datap* locals;
  ulist_vps_dfgp* dfgs;
  vps_dfg* entry;
} vdfg_graph,*vdfg_graphp;

uhstb_decl_tpl(vps_datap);
uhstb_decl_tpl(vdfg_graphp);

typedef struct _vps_mod{
  VPSHEADER;
  uhstb_vps_datap* data;
  uhstb_vdfg_graphp* code;
  ustring* entry;
  ustring* exit;
} vps_mod;

#define VPS_MOD_DATA_TABLE_SIZE 17
#define VPS_MOD_CODE_TABLE_SIZE 17

ulist_decl_tpl(vinstp);

int vinst_to_str(vcontext* ctx,
		 ulist_vinstp* insts);

vps_inst*
vps_inst_new(int instk,
	     usize_t opcode,
	     ustring* label,
	     vps_data* data,
	     vps_dfg* code);

vps_data* vps_num_new(ustring* name,
		      double num,
		      int stk);

vps_data* vps_any_new(ustring* name,
		      int stk);

vdfg_block* vdfg_block_new();

int vdfg_blk2inst(vdfg_block* blk,ulist_vinstp* insts);

#define vdfg_blk_apd(blk,inst)			\
  ulist_vps_instp_append((blk)->insts,inst)

vdfg_graph* vdfg_graph_new();

#define vdfg_grp_cdapd(grp,dfg)			\
  ulist_vps_dfgp_append((grp)->dfgs,dfg)

#define vdfg_grp_dtapd(grp,dt)			\
  ulist_vps_datap_append((grp)->params,dt)

vps_mod* vps_mod_new();

void vps_mod_data_put(vps_mod* mod,vps_data* data);

void vps_mod_code_put(vps_mod* mod,vdfg_graph* code);

vgc_string* vps_dfg2bin(vps_dfg* dfg);

#endif
