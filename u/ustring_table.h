#ifndef _USTRING_TABLE_H_
#define _USTRING_TABLE_H_

#include "umacro.h"
#include "uhstb_tpl.h"
#include "ustring.h"

uhstb_decl_tpl(ustring);

#undef uapi_tpl_decl

typedef uhstb_ustring ustring_table;

#define uapi_tpl_decl

#define ustring_table_new(len)			\
  uhstb_ustring_new(len)

uapi ustring* ucall ustring_table_put(ustring_table* strtb,
			   char*          charp,
			   int            len);

uapi ustring* ucall ustring_table_add(ustring_table* strtb,
			   char*          charp,
			   int            len);

uapi ustring* ucall ustring_concat_by_strtb(ustring_table* strtb,
				 ustring* str1,
				 ustring* str2);
#endif
