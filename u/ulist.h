#ifndef _ULIST_H_
#define _ULIST_H_

#include "ulist_tpl.h"

#define uapi_tpl_decl

ulist_decl_tpl(int);
ulist_decl_tpl(long);
ulist_decl_tpl(float);
ulist_decl_tpl(double);
ulist_decl_tpl(uvoidp);
ulist_decl_tpl(ucharp);
  
#undef uapi_tpl_decl

#endif
