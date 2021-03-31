#include "uerror.h"
#include "ualloc.h"
#include "umempool.h"

static void* alloc_impl(uallocator* allocator,ualloc_size_t size);

static void free_impl(uallocator* allocator,void* ptr){}

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

void umem_pool_init(umem_pool* pool,ualloc_size_t max_size) {
  pool->allocator.alloc = alloc_impl;
  pool->allocator.free = free_impl;
  pool->now_count = 0;
  pool->now_size = 0;
  if (max_size > -1) {
    pool->max_size = max_size;
  } else {
    pool->max_size = UMEM_POOL_MAX_SIZE;
  }
  pool->node = NULL;
}

void* umem_pool_alloc(umem_pool* pool,ualloc_size_t size){
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
      pool->now_count++;
      pool->now_size += curr_node->total_size;
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
  return data;
}

static void* alloc_impl(uallocator* allocator,ualloc_size_t size)
{
  umem_pool* mp = (umem_pool*)allocator;
  return umem_pool_alloc(mp,size);
}

void umem_pool_clean(umem_pool* pool){
  umem_node* node;
  
  node = pool->node;
  ulog("umem pool clean before count:%d,size:%d",
       pool->now_count,pool->now_size);
  while (node) {
    umem_node* temp = node;
    node = node->next;
    if (pool->now_size > pool->max_size) {
      pool->node = temp;
      pool->now_count--;
      pool->now_size -= temp->total_size;
      ufree(temp);
    } else {
      temp->remain_size = temp->total_size;
      temp->index = temp->mem;
    }
  }
  ulog("umem pool clean after count:%d,size:%d",
	pool->now_count,pool->now_size);
}
