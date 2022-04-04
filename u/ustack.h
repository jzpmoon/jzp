#ifndef _USTACK_H_
#define _USTACK_H_

#include "udef.h"
#include "ustack_tpl.h"

#define uapi_tpl_decl

ustack_decl_tpl(int)
ustack_decl_tpl(long)
ustack_decl_tpl(float)
ustack_decl_tpl(double)
ustack_decl_tpl(uvoidp)

#undef uapi_tpl_decl

#endif
