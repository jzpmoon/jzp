#ifndef _UMEMPOOL_H_
#define _UMEMPOOL_H_

typedef struct _umem_node{
  umem_node* next;
  int size;
  unsigned char mem[1];
} umem_node;

typedef struct _umem_pool{
  int count;
  umem_node* node;
} umem_pool;

void* umem_pool_alloc(umem_pool* pool,int size);

void umem_pool_clean(umem_pool* pool);

#endif
