#include "lobj.h"
#include "leval.h"

int lcfun_parse(vcontext* ctx){
  return 0;
}

int lcfun_stream_new(vcontext* ctx){
  char* path = "./test.txt";
  vm* vm = ctx->vm;
  vgc_stack* stack = (vgc_stack*)
    vslot_ref_get(ctx->stack);
  lstream_new_by_file(vm->heap,
		      stack,
		      path);
  return 0;
}

int lentry(vcontext* ctx){
  vm* vm = ctx->vm;
  ustring* string;
  vgc_cfun* cfun;
  
  ulog("begin lentry");
  string = ustring_new_by_charp("make-stream");
  if(!string){
    uabort("ustring new error!");
  }
  cfun = vgc_cfun_new(vm->heap,
		      0,
		      lcfun_stream_new,
		      area_static);
  vm_obj_put(ctx->vm,
	     string,
	     (vgc_obj*)cfun);
  ulog("end   lentry");
  
  return 0;
}

int lstartup(vm* vm){
  vgc_cfun* entry = vgc_cfun_new(vm->heap,0,lentry,area_static);
  vcontext_execute(vm->context,(vgc_obj*)entry);
  return 0;
}
