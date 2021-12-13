#include "uhstb_tpl.h"

typedef struct _vir{
  int ir_no;
  char* ir_str;
  int ir_code;
  int ir_len;
} vir;

uhstb_decl_tpl(vir);

typedef uhstb_vir virtb;

extern virtb* irtb;

virtb* virtb_new();

int virtb_put(virtb* irtb,int ir_no,char* ir_str,int ir_code,int ir_len);

int virtb_load();

int ir2target();
