#ifndef _UMEMPOOL_H_
#define _UMEMPOOL_H_

#include "ualloc.h"

#define UMEM_NODE_SIZE 4*1024
#define UMEM_POOL_MAX_SIZE 4*1024

typedef struct _umem_node{
  struct _umem_node* next;
  int total_size;
  int remain_size;
  unsigned char* index;
  unsigned char mem[1];
} umem_node;

typedef struct _umem_pool{
  uallocator allocator;
  int now_count;
  int now_size;
  int max_size;
  umem_node* node;
} umem_pool;

void umem_pool_init(umem_pool* pool,ualloc_size_t max_size);

void* umem_pool_alloc(umem_pool* pool,ualloc_size_t size);

void umem_pool_clean(umem_pool* pool);

#endif
