#ifndef _VPASS_H_
#define _VPASS_H_

#include "ulist.h"
#include "vgc_obj.h"

#define VPSHEADER \
  int t;

#define VDFGHEADER \
  VPSHEADER	   \
  ulist* link

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
} vps_t;

typedef struct _vps_data{
  VPSHEADER;
  int dtk;
  int stk;
  int idx;
  ustring* name;
  union {
    ulist* array;
    double number;
    int integer;
  } u;
} vps_data;

#define vopdnon (-1)

typedef struct _vinst{
  usize_t opcode;
  usize_t operand;
} vinst;

typedef struct _vps_inst{
  VPSHEADER;
  int instk;
  usize_t opcode;
  ustring* label;
  union {
    vps_data* data;
    struct _vps_dfg* code;
  } u;
} vps_inst;

typedef struct _vps_dfg{
  VDFGHEADER;
} vps_dfg;

typedef struct _vdfg_block{
  VDFGHEADER;
  ulist* insts;
} vdfg_block;

typedef struct _vdfg_graph{
  VDFGHEADER;
  ustring* name;
  ulist* dts;
  ulist* dfgs;
  vps_dfg* entry;
} vdfg_graph;

uhstb_decl_tpl(vps_data);
uhstb_decl_tpl(vdfg_graph);

typedef struct _vps_mod{
  VPSHEADER;
  uhstb_vps_data* data;
  uhstb_vdfg_graph* code;
} vps_mod;

#define VPS_MOD_DATA_TABLE_SIZE 17
#define VPS_MOD_CODE_TABLE_SIZE 17

int vinst_to_str(vcontext* ctx,
		 ulist* insts);

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

#define vdfg_blk_apd(blk,inst)			\
  (blk)->insts=ulist_append((blk)->insts,inst)

vdfg_graph* vdfg_graph_new();

#define vdfg_grp_cdapd(grp,dfg)			\
  (grp)->dfgs=ulist_append((grp)->dfgs,dfg)

#define vdfg_grp_dtapd(grp,dt)			\
  (grp)->dts=ulist_append((grp)->dts,dt)

vps_mod* vps_mod_new();

void vps_mod_data_put(vps_mod* mod,vps_data* data);

void vps_mod_code_put(vps_mod* mod,vdfg_graph* code);

vgc_string* vps_dfg2bin(vps_dfg* dfg);

#endif
