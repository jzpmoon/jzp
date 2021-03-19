#ifndef _LREADER_H_
#define _LREADER_H_

#include "uhstb_tpl.h"
#include "ltoken.h"

uhstb_decl_tpl(ustream);

typedef struct _lreader{
  umem_pool mp;
  ustring_table* symtb;
  ustring_table* strtb;
  struct _uhstb_last_attr* attrtb;
  uhatb_ustream* streamtb;
} lreader;

#endif
