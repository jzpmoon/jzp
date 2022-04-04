#ifndef _UHSTB_H_
#define _UHSTB_H_

#include "udef.h"
#include "uhstb_tpl.h"

#define uapi_tpl_decl

uhstb_decl_tpl(int);
uhstb_decl_tpl(short);
uhstb_decl_tpl(long);
uhstb_decl_tpl(float);
uhstb_decl_tpl(double);
uhstb_decl_tpl(uvoidp);

#undef uapi_tpl_decl

#endif
