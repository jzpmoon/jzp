#ifndef _UALLOC_H_
#define _UALLOC_H_

#include <malloc.h>
#include <stdlib.h>
#include "umacro.h"

#define uif(test,t,e)				\
  if(test){t}else{e}

#define unew(ptr,size,err)			\
  uif(!((ptr)=malloc(size)),err,(void)0;)

#define ualloc(size) malloc(size)

#define ufree(ptr) free(ptr)

#define BOUNDARY (sizeof(void*))
#define ALIGN(size,align) ((size+align-1)&(~(align-1)))

#define ALIGN_BOUNDARY(size) ALIGN(size,BOUNDARY)

#define TYPE_SIZE_OF(stype,mtype,length)	\
  sizeof(stype)+sizeof(mtype)*((length)-1)

#define uoffsetof(stype,member) offsetof(stype,member)

typedef size_t ualloc_size_t;
typedef struct _uallocator uallocator;
typedef void* (ucall *ualloc_ft)(uallocator* allocator,ualloc_size_t size);
typedef void* (ucall *uallocx_ft)(uallocator* allocator,char* data,
			   ualloc_size_t size);
typedef void (ucall *ufree_ft)(uallocator* allocator,void* ptr);
typedef void (ucall *uclean_ft)(uallocator* allocator);

#define UALLOCATOR_HEADER \
  ualloc_ft alloc;	  \
  uallocx_ft allocx;	  \
  ufree_ft free;	  \
  uclean_ft clean

struct _uallocator{
  UALLOCATOR_HEADER;
};

extern uapi uallocator u_global_allocator;

#endif
