#include "vpass.h"

vdfg_graph* vdfg_graph_new(){
  vdfg_graph* g = ualloc(sizeof(vdfg_graph));
  if(g){
    g->t = vdfgk_graph;
  }
  return g;
}

vgc_string* vpass_dfg2bin(vdfg* dfg){
  
}
