#include "ulist_tpl.c"
#include "vclosure.h"

ulist_def_tpl(vclosurep);

UDEFUN(UFNAME vclosure_new,
       UARGS (vps_cntr* vps),
       URET vclosure*)
UDECLARE
  vclosure* closure;
  uallocator* allocator;
UBEGIN
  allocator = (uallocator*)&vps->mp;
  closure = allocator->alloc(allocator,sizeof(vclosure));
  if (closure) {
    closure->fields = ulist_vps_datap_alloc(allocator);
    if (!closure->fields) {
      goto err;
    }
    closure->init = NULL;
    closure->parent = NULL;
    closure->childs = ulist_vclosurep_alloc(allocator);
    if (!closure->childs) {
      goto err;
    }
  }

  return closure;
 err:
  umem_pool_clean(&vps->mp);
  return NULL;
UEND
