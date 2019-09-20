#ifndef _USTRING_TABLE_H_
#define _USTRING_TABLE_H_

#include "uhash_table.h"

ustring* ustring_table_put(ustring_table* strtb,
			   char*          charp,
			   int            len);

#endif
