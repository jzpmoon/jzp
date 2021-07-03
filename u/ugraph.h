#ifndef _UGRAPH_H_
#define _UGRAPH_H_

#include "ulist_tpl.h"

typedef struct _ugnode ugnode,* ugnodep;

ulist_decl_tpl(ugnodep);

#define UGNODEHEADER	      \
  ulist_ugnodep* in_degree;   \
  ulist_ugnodep* out_degree

struct _ugnode{
  UGNODEHEADER;
};

#define UGRAPHHEADER				\
  ulist_ugnodep* nodes

typedef struct _ugraph{
  UGRAPHHEADER;
} ugraph;

#define ugraph_node_link(from,to)			\
  do{							\
    ulist_ugnodep_append((from)->out_degree,to);	\
    ulist_ugnodep_append((to)->in_degree,from);		\
  } while(0);

#define ugraph_node_add(graph,node)		\
  do{						\
    ulist_ugnodep_append((graph)->nodes,node);	\
  } while(0);

#define ugraph_node_init(node,allocator)			\
  do{								\
    (node)->out_degree = ulist_ugnodep_alloc(allocator);	\
    (node)->in_degree = ulist_ugnodep_alloc(allocator);		\
  } while(0);

#endif
