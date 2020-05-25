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
     vpsk_mod,
     vpsk_dt,
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
};

typedef struct _vps_t{
  VPSHEADER;
} vps_t;

typedef struct _vps_data{
  VPSHEADER;
  int data_type;
  ustring* name;
  union {
    ulist* array;
    double number;
    int integer;
  } u;
} vps_data;

typedef struct _vps_dfg{
  VDFGHEADER;
} vps_dfg;

typedef struct _vinst{
  int inst_type;
  usize_t opcode;
  usize_t operand;
  ustring* label;
  union {
    vps_data* data;
    vps_dfg* code;
  } u;
} vinst;

typedef struct _vdfg_block{
  VDFGHEADER;
  ulist* insts;
} vdfg_block;

typedef struct _vdfg_graph{
  VDFGHEADER;
  ustring* name;
  ulist* dfgs;
  vps_dfg* entry;
} vdfg_graph;

typedef struct _vps_mod{
  VPSHEADER;
  uhash_table* data;
  uhash_table* code;
} vps_mod;

#define VPS_MOD_DATA_TABLE_SIZE 17
#define VPS_MOD_CODE_TABLE_SIZE 17

vinst*
vinst_new(usize_t code,usize_t operand);

int vinst_to_str(vcontext* ctx,
		 ulist* insts);

vdfg_graph* vdfg_graph_new();

vps_mod* vps_mod_new();

vgc_string* vps_dfg2bin(vps_dfg* dfg);

#endif
