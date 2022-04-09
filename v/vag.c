#include "uerror.h"
#include "ualloc.h"
#include "uhstb_tpl.c"
#include "ulist_tpl.c"
#include "vgenbc.h"
#include "vparser.h"
#include "vag.h"
#include "_vtemp.attr"

uhstb_def_tpl(vir);
uhstb_def_tpl(vag_tplp);
ulist_def_tpl(vir_nterm);

void vcall vattr_init(vreader* reader)
{
  vattr_file_concat_init(reader);
}

virtb* virtb_new()
{
  virtb* irtb;
  irtb = uhstb_vir_new(-1);
  return irtb;
}

int ucall vir_comp(vir* ir1,vir* ir2)
{
  return (int)(ir1->ir_str - ir2->ir_str);
}

int virtb_put(virtb* irtb,vir ir)
{
  int retval;

  retval = uhstb_vir_put(irtb,ir.ir_str->hash_code,&ir,NULL,NULL,vir_comp);

  return retval;
}

int virtb_load(vreader* reader,virtb* irtb)
{
  ustring* ir_cid;
  ustring* ir_str;
  vir ir;
  int retval;
  
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
  retval = virtb_put(irtb,ir);                            \
  if (retval == 1) {                                      \
    uabort("ir inst already exists!");                    \
  } else if (retval != 0) {                               \
    uabort("sirtb put error!");                           \
  }
#include "vbytecode.h"
  VBYTECODE;
#undef DF
  return 0;
}

ulrgram* vfile2gram(vreader* reader,char* file_path)
{
  ustring* file_path_str;
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
  file_path_str = ustring_table_put(reader->symtb,file_path,-1);
  if (!file_path_str) {
    uabort("file path put symtb error!");
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
  vtoken_state_reset(ts,file_path_str);
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

int vir_nterms_put(vir_nterms* nterms,ustring* name)
{
  uset* set;
  ucursor c;
  vir_nterm n;

  set = (uset*)nterms;
  set->iterate(&c);
  while (1) {
    vir_nterm* nterm = set->next(set,&c);
    if (!nterm) {
      break;
    }
    if (nterm->name == name) {
      return nterm->no;
    }
  }
  n.name = name;
  n.no = nterms->len;
  if (ulist_vir_nterm_append(nterms,n)) {
    return -1;
  }
  return nterms->len;
}

vir_nterm* vir_nterm_get(vir_nterms* nterms,ustring* name)
{
  uset* set;
  ucursor c;

  set = (uset*)nterms;
  set->iterate(&c);
  while (1) {
    vir_nterm* nterm = set->next(set,&c);
    if (!nterm) {
      break;
    }
    if (nterm->name == name) {
      return nterm;
    }
  }
  return NULL;
}

int main(int argc,char** args)
{
  ulrgram* gram;
  vreader* reader;
  ulog_conf log_conf;

  log_conf.log_fn = "vag.log";
  log_conf.power = UTRUE;
  log_conf.line = 100;
  ulog_init(&log_conf);
  
  if(argc == 1){
    ulog("no input file!");
    return 0;
  }
 
  reader = vreader_easy_new(vattr_init);
  if (!reader) {
    uabort("reader new error!");
  }
  gram = vfile2gram(reader,args[1]);
  return 0;
}
