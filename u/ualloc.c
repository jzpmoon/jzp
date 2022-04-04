#include "ualloc.h"

static void* ucall default_alloc(uallocator* allocator,ualloc_size_t size)
{
  return ualloc(size);
}

static void* ucall default_allocx(uallocator* allocator,char* data,
			    ualloc_size_t size)
{
  return ualloc(size);
}


static void ucall default_free(uallocator* allocator,void* ptr)
{
  ufree(ptr);
}

static void ucall default_clean(uallocator* allocator)
{}

uapi uallocator u_global_allocator = {default_alloc,default_allocx,default_free,
  default_clean};
