#include <string.h>
#include "ustring.h"
#include "ustring_table.h"

static void* ustrtb_key_put(void* key){
  ustring* str = (ustring*)key;
  ustring* new_str;
  new_str = ustring_new(str->value,
			str->len,
			str->hash_code);
  return new_str;
}

ustring* ustring_table_put(ustring_table* strtb,
			   int            tblen,
			   char*          charp,
			   int            len){
  unsigned int hscd = len > 0 ? data_hscd(charp,len) : string_hscd(charp);
  int      _len     = len > 0 ? len : strlen(charp);
  ustring  str      = (ustring){charp,_len,hscd};
  ustring* new_str  = uhash_table_put(strtb,
				      hscd % tblen,
				      &str,
				      ustrtb_key_put,
				      (uhstb_comp_ft)ustring_comp);
  return new_str;
}