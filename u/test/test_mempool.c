#include <stdio.h>
#include <stddef.h>
#include "umempool.h"

int main(){
  umem_pool pool;
  int i = 0;
  
  umem_pool_init(&pool);
  while (i < 10) {
    umem_pool_alloc(&pool,1024);
    i++;
  }

  umem_pool_clean(&pool);
  
  return 0;
}
