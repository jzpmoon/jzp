#ifndef _VPASS_H_
#define _VPASS_H_

#include "ulist.h"
#include "vgc_obj.h"
#include "vm.h"

#define VPSHEADER \
  int t;

#define VDFGHEADER \
  VPSHEADER	   \
  ulist* link

enum{
  vpsk_dt,
  vpsk_mod,
  vpsk_inst,
  vpsk_dfg,
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
};

typedef struct _vps_t{
  VPSHEADER;
} vps_t;

typedef struct _vps_data{
  VPSHEADER;
  int dtk;
  ustring* name;
  union {
    ulist* array;
    double number;
    int integer;
  } u;
} vps_data;

typedef struct _vps_mod{
  VPSHEADER;
  uhash_table* data;
  uhash_table* code;
} vps_mod;

#define vopdnon (-1)

typedef struct _vinst{
  VPSHEADER;
  int instk;
  usize_t opcode;
  usize_t operand;
  ustring* label;
  union {
    vps_data* data;
    struct _vps_dfg* code;
  } u;
} vinst;

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

#define VPS_MOD_DATA_TABLE_SIZE 17
#define VPS_MOD_CODE_TABLE_SIZE 17

vinst*
vinst_new(int instk,
	  usize_t opcode,
	  usize_t operand,
	  ustring* label,
	  vps_data* data,
	  vps_dfg* code);

int vinst_to_str(vcontext* ctx,
		 ulist* insts);

vps_data* vps_num_new(ustring* name,
		      double num);

vps_data* vps_any_new(ustring* name);

vdfg_block* vdfg_block_new();

#define vdfg_block_append(blk,inst)		\
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
