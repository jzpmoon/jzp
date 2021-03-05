#include "ualloc.h"

static void* default_alloc(uallocator* allocator,ualloc_size_t size)
{
  return ualloc(size);
}

static void default_free(uallocator* allocator,void* ptr)
{
  ufree(ptr);
}

uallocator u_global_allocator = {default_alloc,default_free};
