#include "lobj.h"
#include "leval.h"

int lcfun_parse(vcontext* ctx){
  vslot* slotp_stream = vcontext_args_get(ctx,1);
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
  vgc_stack* stack = vslot_ref_get(ctx->stack);
  ustring* string;
  vslot* cfun;
  
  ulog("begin lentry");
  string = ustring_new_by_charp("make-stream");
  if(!string){
    uabort("ustring new error!");
  }
  cfun = vgc_cfun_new(vm->heap,
		      stack,
		      0,
		      lcfun_stream_new,
		      area_static);
  vm_obj_put(ctx->vm,
	     string,
	     cfun);
  ulog("end   lentry");
  
  return 0;
}

int lstartup(vm* vm){
  vcontext* ctx = vm->context;
  vgc_stack* stack = vslot_ref_get(ctx->stack);
  vslot* entry = vgc_cfun_new(vm->heap,stack,0,lentry,area_static);
  vcontext_execute(vm->context,entry);
  return 0;
}
