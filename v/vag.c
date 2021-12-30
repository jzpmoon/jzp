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
  return (int)(ir1->ir_str - ir2->ir_str);
}

int virtb_put(virtb* irtb,vir ir)
{
  int retval;

  retval = uhstb_vir_put(irtb,ir.ir_str->hash_code,&ir,NULL,NULL,vir_comp);
  if (!retval) {
    return -1;
  }
  return 0;
}

int virtb_load(vreader* reader,virtb* irtb)
{
  ustring* ir_cid;
  ustring* ir_str;
  vir ir;
  
#define DF(no,str,code,len,oct)			                  \
  ir_cid = ustring_table_put(reader->symtb,#no,-1);       \
  if (!ir_cid) {                                          \
	uabort("put symtb ir_no error!");                     \
  }                                                       \
  ir_str = ustring_table_put(reader->symtb,str,-1);       \
  if (!ir_str) {                                          \
	uabort("put symtb ir_str error!");                    \
  }                                                       \
  ir.ir_no = no;                                          \
  ir.ir_cid = ir_cid;                                     \
  ir.ir_str = ir_str;                                     \
  ir.ir_code = code;                                      \
  ir.ir_len = len;                                        \
  ir.ir_oct = oct;                                        \
  if (!virtb_put(irtb,ir)) {                              \
    return -1;					                          \
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
  virtb* sirtb;
  virtb* dirtb;
  ulrgram* gram;
  
  ts = vreader_from(reader);
  if (!ts) {
    uabort("reader from error!");
  }
  file = fopen(file_path,"r");
  if(!file){
    uabort("open file error!");
  }
  sirtb = virtb_new();
  if (!sirtb) {
    uabort("virtb new error!");
  }
  dirtb = virtb_new();
  if (!dirtb) {
    uabort("virtb new error!");
  }
  if (virtb_load(reader,sirtb)) {
    uabort("virtb load error!");
  }
  gram = ulrgram_new();
  if (!gram) {
    uabort("lr gram new error!");
  }
  ireq.reader = reader;
  ireq.sirtb = sirtb;
  ireq.dirtb = dirtb;
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
