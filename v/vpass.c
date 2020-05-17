#include "vpass.h"

vdfg_block* vdfg_block_new(){
  vdfg_block* b = ualloc(sizeof(vdfg_block));
  if(b){
    b->t = vdfgk_block;
    b->link = NULL;
    b->insts = NULL;
  }
  return b;
}

vdfg_graph* vdfg_graph_new(){
  vdfg_graph* g = ualloc(sizeof(vdfg_graph));
  if(g){
    g->t = vdfgk_graph;
    g->link = NULL;
    g->name = NULL;
    g->dfgs = NULL;
    g->entry = NULL;
  }
  return g;
}

vgc_string* vpass_dfg2bin(vdfg* dfg){
  return NULL;
}
