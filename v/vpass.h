#ifndef _VPASS_H_
#define _VPASS_H_

#include "ulist.h"
#include "vgc_obj.h"

#define VDFGHEADER \
  int t; \
  ulist* link

enum{
     vdfgk_block,
     vdfgk_graph,
};

typedef struct _vdfg{
  VDFGHEADER;
} vdfg;

typedef struct _vdfg_block{
  VDFGHEADER;
  ulist* insts;
} vdfg_block;

typedef struct _vdfg_graph{
  VDFGHEADER;
  ustring* name;
  ulist* dfgs;
  vdfg* entry;
} vdfg_graph;

vdfg_graph* vdfg_graph_new();

vgc_string* vpass_dfg2bin(vdfg* dfg);

#endif
