#include "lobj.h"
#include "leval.h"

LDEFUN(s_exp_parse,"s-exp-parse",1,{
    vvm* vm = ctx->vm;
    vgc_stack* stack = vslot_ref_get(ctx->stack);
    vslot* slotp_parser;
    lparser* parser;
    vslot* slotp_stream;
    lstream* stream;
    vslot* slotp_exp;
    
    slotp_stream = vcontext_args_get(ctx,0);
    if(!slotp_stream){
      uabort("s-exp-parse:get args error!");
    }
    stream = vslot_ref_get(*slotp_stream);

    slotp_parser = vvm_obj_get(vm,"s-exp-parser");
    if(!slotp_parser){
      uabort("s-exp-parse:get parser error!");
    }
    parser = vslot_ref_get(*slotp_parser);
    
    ltoken_state_init(parser->ts,stream->stream);

    slotp_exp = lparser_parse(parser->ts,vm->heap,stack);
    lparser_exp_log(slotp_exp);
    
    vcontext_call_return(ctx,NULL);
    return 0;
  });

LDEFUN(make_stream,"make-stream",1,{
    vvm* vm = ctx->vm;
    vslot* slotp_make_stream;
    vgc_stack* stack = vslot_ref_get(ctx->stack);
    vslot* slotp_str;
    vgc_str* str;
    char* file_path;
    slotp_str = vcontext_args_get(ctx,0);
    if(!slotp_str){
      uabort("make-stream:get args error!");
    }
    str = vslot_ref_get(*slotp_str);
    file_path = vgc_str_charp_get(str);
    slotp_make_stream = lstream_new_by_file(vm->heap,
					    stack,
					    file_path);
    if(!slotp_make_stream){
      uabort("make-stream:new stream error!");
    }
    ulog("make-stream");
    vcontext_call_return(ctx,slotp_make_stream);
    return 0;
  });

void leval_init(vvm* vm){
  vgc_heap* heap = vm->heap;
  vcontext* ctx = vm->context;
  vgc_stack* stack = vslot_ref_get(ctx->stack);
  vslot* slotp_parser;
  
  slotp_parser = lparser_new(heap,stack);
  LVAR_INIT(vm,"s-exp-parser",slotp_parser);
  
  LFUN_INIT(vm,make_stream);
  LFUN_INIT(vm,s_exp_parse);
}

int lentry(vcontext* ctx){
  vvm* vm = ctx->vm;
  vgc_stack* stack = vslot_ref_get(ctx->stack);
  vslot* slotp_args;
  vslot* slotp_make_stream;
  vslot* slotp_s_exp_parse;
  
  slotp_args = vcontext_args_get(ctx,0);
  if(!slotp_args){
    uabort("lentry:get args error!");
  }
  
  leval_init(vm);

  if(vgc_stack_push(stack,*slotp_args) == -1){
    uabort("lentry:stack overflow!");
  }
  
  slotp_make_stream = vvm_obj_get(vm,"make-stream");
  if(!slotp_make_stream){
    uabort("lentry:get make-stream error!");
  }
  slotp_s_exp_parse = vvm_obj_get(vm,"s-exp-parse");
  if(!slotp_s_exp_parse){
    uabort("lentry:get s-exp-parse error!");
  }
  
  vcontext_execute(ctx,slotp_make_stream);
  vcontext_execute(ctx,slotp_s_exp_parse);
  
  return 0;
}

int lstartup(vvm* vm,vslot* args){
  vcontext* ctx = vm->context;
  vgc_stack* stack = vslot_ref_get(ctx->stack);
  vslot* slotp_entry = vgc_cfun_new(vm->heap,stack,1,lentry,area_static);
  if(!slotp_entry){
    uabort("lstartup:cfun new error!");
  }
  if(vgc_stack_push(stack,*args) == -1){
    uabort("lstartup:stack overflow!");
  }
  vcontext_execute(vm->context,slotp_entry);
  return 0;
}
