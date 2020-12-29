#include "uerror.h"
#include "ualloc.h"
#include "umempool.h"

#define UMEM_NODE_SIZE 4*1024

umem_node* umem_node_new(int size){
  int align_size;
  int offset_size;
  umem_node* node;

  offset_size = uoffsetof(umem_node,mem);
  align_size = size + offset_size;
  align_size = ALIGN(align_size,UMEM_NODE_SIZE);
  node = ualloc(align_size);
  if (node) {
    node->next = NULL;
    node->total_size =
      node->remain_size =
      align_size - offset_size;
    node->index = node->mem;
  }
  return node;
}

void umem_pool_init(umem_pool* pool) {
  pool->count = 0;
  pool->node = NULL;
}

void* umem_pool_alloc(umem_pool* pool,int size){
  umem_node* curr_node;
  umem_node* prev_node;
  int align_size;
  void* data;
  
  curr_node = pool->node;
  prev_node = NULL;
  align_size = ALIGN_BOUNDARY(size);
  while (1) {
    if (!curr_node) {
      curr_node = umem_node_new(size);
      if (!curr_node) {
	return NULL;
      }
      if (prev_node) {
	prev_node->next = curr_node;
      } else {
	pool->node = curr_node;
      }
      pool->count++;
      break;
    }
    if (curr_node->remain_size >= align_size) {
      break;
    }
    prev_node = curr_node;
    curr_node = curr_node->next;
  }
  data = curr_node->index;
  curr_node->index += align_size;
  curr_node->remain_size -= align_size;
  ulog1("align_size :%d",align_size);
  ulog1("size       :%d",size);
  ulog1("remain_size:%d",curr_node->remain_size);  
  return data;
}

void umem_pool_clean(umem_pool* pool){
  umem_node* node;

  node = pool->node;
  while (node) {
    ulog1("remain_size:%d",node->remain_size);
    node->remain_size = node->total_size;
    node->index = node->mem;
    node = node->next;
  }
}
