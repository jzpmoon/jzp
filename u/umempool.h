#ifndef _UMEMPOOL_H_
#define _UMEMPOOL_H_

typedef struct _umem_node{
  struct _umem_node* next;
  int total_size;
  int remain_size;
  void* index;
  unsigned char mem[1];
} umem_node;

typedef struct _umem_pool{
  int now_count;
  int now_size;
  int max_size;
  umem_node* node;
} umem_pool;

void umem_pool_init(umem_pool* pool,int max_size);

void* umem_pool_alloc(umem_pool* pool,int size);

void umem_pool_clean(umem_pool* pool);

#endif
