#ifndef _USTRING_TABLE_H_
#define _USTRING_TABLE_H_

#include "uhstb_tpl.h"
#include "ustring.h"

uhstb_decl_tpl(ustring);

typedef struct _ustring_table ustring_table;

struct _ustring_table{
  uallocator allocator;
  uhstb_ustring* strtb;
};

ustring_table* ustring_table_new(int len);

ustring* ustring_table_put(ustring_table* strtb,
			   char*          charp,
			   int            len);

ustring* ustring_table_add(ustring_table* strtb,
			   char*          charp,
			   int            len);

ustring* ustring_concat_by_strtb(ustring_table* strtb,
				 ustring* str1,
				 ustring* str2);

ustring* ustring_concatx_by_strtb(ustring_table* strtb,
				  ustring* str1,
				  ustring* str2,
				  char* sep);

#endif
