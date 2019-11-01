#include "leval.h"

int lcfun_parse(vcontext* ctx){
  return 0;
}

int lentry(vcontext* ctx){
  ulog("lentry");
  return 0;
}

int lstartup(vm* vm){
  vgc_cfun* entry = vgc_cfun_new(vm->heap,0,lentry,area_static);
  vcontext_execute(vm->context,(vgc_obj*)entry);
  return 0;
}
