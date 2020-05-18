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
     vpsk_dfg,
     vdfgk_blk,
     vdfgk_grp,
};

typedef struct _vps_t{
  VPSHEADER;
} vps_t;

typedef struct _vps_data{
  VPSHEADER;
  ustring* name;
  vgc_obj* value;
} vps_data;

typedef struct _vps_dfg{
  VDFGHEADER;
} vps_dfg;

typedef struct _vinst{
  usize_t opcode;
  usize_t operand;
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

vinst*
vinst_new(usize_t code,usize_t operand);

int vinst_to_str(vcontext* ctx,
		 ulist* insts);

vdfg_graph* vdfg_graph_new();

vgc_string* vps_dfg2bin(vps_dfg* dfg);

#endif
