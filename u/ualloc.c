#include "ualloc.h"

static void* default_alloc(uallocator* allocator,ualloc_size_t size)
{
  return ualloc(size);
}

static void* default_allocx(uallocator* allocator,char* data,
			    ualloc_size_t size)
{
  return ualloc(size);
}


static void default_free(uallocator* allocator,void* ptr)
{
  ufree(ptr);
}

static void default_clean(uallocator* allocator)
{}

uallocator u_global_allocator = {default_alloc,default_allocx,default_free,
  default_clean};
