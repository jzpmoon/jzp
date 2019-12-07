#ifndef _USTRING_TABLE_H_
#define _USTRING_TABLE_H_

#include "uhash_table.h"
#include "ustring.h"

typedef uhash_table ustring_table;

ustring* ustring_table_put(ustring_table* strtb,
			   int            tblen,
			   char*          charp,
			   int            len);

ustring* ustring_table_add(ustring_table* strtb,
			   int            tblen,
			   char*          charp,
			   int            len);
#endif
