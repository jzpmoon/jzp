#ifndef _USTRING_TABLE_H_
#define _USTRING_TABLE_H_

#include "uhstb_tpl.h"
#include "ustring.h"

uhstb_decl_tpl(ustring);

typedef uhstb_ustring ustring_table;

#define ustring_table_new(len)			\
  uhstb_ustring_new(len)

ustring* ustring_table_put(ustring_table* strtb,
			   char*          charp,
			   int            len);

ustring* ustring_table_add(ustring_table* strtb,
			   char*          charp,
			   int            len);

#endif
