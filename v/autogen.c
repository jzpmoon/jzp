#include "uhstb_tpl.c"
#include "vgenbc.h"
#include "autogen.h"

uhstb_def_tpl(vir);

virtb* irtb;

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

int virtb_load()
{
  irtb = virtb_new();
  if (!irtb) {
    return -1;
  }
#define DF(no,str,code,len)			\
  if(!virtb_put(irtb,no,str,code,len)){		\
    return -1;					\
  }
#include "vbytecode.h"
  VBYTECODE;
#undef DF
  return 0;
}

int ir2target()
{
  return 0;
}
