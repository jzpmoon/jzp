#ifndef _UGRAPH_H_
#define _UGRAPH_H_

#include "umacro.h"
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
    if (ulist_ugnodep_append((from)->out_degree,to)) {	\
      uabort("graph node link error!");			\
    }							\
    if (ulist_ugnodep_append((to)->in_degree,from)) {	\
      uabort("graph node link error!");			\
    }							\
  } while(0);

#define ugraph_node_add(graph,node)			\
  do{							\
    if (ulist_ugnodep_append((graph)->nodes,node)) {	\
      uabort("graph node add error!");			\
    }							\
  } while(0);

#define ugraph_node_init(node,allocator)			\
  do{								\
    (node)->out_degree = ulist_ugnodep_alloc(allocator);	\
    if (!(node)->out_degree) {					\
      uabort("graph node init error1");				\
    }								\
    (node)->in_degree = ulist_ugnodep_alloc(allocator);		\
    if (!(node)->in_degree) {					\
      uabort("graph node init error1");				\
    }								\
  } while(0);

#define ugraph_node_clean(node)				\
  do{							\
    ulist_ugnodep_dest((node)->out_degree,NULL);	\
    ulist_ugnodep_dest((node)->in_degree,NULL);		\
  } while(0)

#define ugraph_init(graph,allocator)			\
  do{							\
    (graph)->nodes = ulist_ugnodep_alloc(allocator);	\
    if (!(graph)->nodes) {				\
      uabort("graph init error!");			\
    }							\
  } while(0);

#define ugraph_clean(graph)			\
  do{						\
    ulist_ugnodep_dest((graph)->nodes,NULL);	\
  } while(0)

#endif
