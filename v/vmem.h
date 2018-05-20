#ifndef _VMEM_H_
#define _VMEM_H_

#include "ualloc.h"

#define vnew(ptr,size,err) unew(ptr,size,err)
#define vfree(ptr) ufree(ptr)

#endif
