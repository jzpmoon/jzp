#include "uhstb_tpl.c"
#include "vgenbc.h"
#include "vparser.h"
#include "vag.h"

uhstb_def_tpl(vir);

virtb* virtb_new()
{
  virtb* irtb;
  irtb = uhstb_vir_new(-1);
  return irtb;
}

int vir_comp(vir* ir1,vir* ir2)
{
  return ir1->ir_no - ir2->ir_no;
}

int virtb_put(virtb* irtb,int ir_no,char* ir_str,int ir_code,int ir_len)
{
  vir ir;
  int retval;
  
  ir.ir_no = ir_no;
  ir.ir_str = ir_str;
  ir.ir_code = ir_code;
  ir.ir_len = ir_len;

  retval = uhstb_vir_put(irtb,ir_no,&ir,NULL,NULL,vir_comp);
  if (!retval) {
    return -1;
  }
  return 0;
}

int virtb_load(virtb* irtb)
{
#define DF(no,str,code,len)			\
  if(!virtb_put(irtb,no,str,code,len)){		\
    return -1;					\
  }
#include "vbytecode.h"
  VBYTECODE;
#undef DF
  return 0;
}

ulrgram* vfile2gram(vreader* reader,char* file_path)
{
  FILE* file;
  vtoken_state* ts;
  vir_attr_req ireq;
  vast_attr_res res;
  vast_obj* ast_obj;
  virtb* irtb;
  ulrgram* gram;
  
  ts = vreader_from(reader);
  if (!ts) {
    uabort("reader from error!");
  }
  file = fopen(file_path,"r");
  if(!file){
    uabort("open file error!");
  }
  irtb = virtb_new();
  if (!irtb) {
    uabort("virtb new error!");
  }
  if (virtb_load(irtb)) {
    uabort("virtb load error!");
  }
  gram = ulrgram_new();
  if (!gram) {
    uabort("lr gram new error!");
  }
  ireq.reader = reader;
  ireq.irtb = irtb;
  ireq.gram = gram;
  vtoken_state_reset(ts,file);
  while (1) {
    ast_obj = vparser_parse(ts);
    if (!ast_obj){
      break;
    }
    vast_obj_log(ast_obj);
    ireq.ast_obj = ast_obj;
    vast2obj((vast_attr_req*)&ireq,&res);
  }
  vtoken_state_close(ts);
  
  return gram;
}
