#include <string.h>
#include "ustring.h"
#include "uhstb_tpl.c"
#include "ustring_table.h"

uhstb_def_tpl(ustring);

static ustring ustrtb_key_put(ustring* key){
  ustring* str = key;
  char* charp;
  ustring new_str;
  charp = ucharp_new(NULL,
		     str->value,
		     str->len);
  if(!charp){
    uabort("ucharp_new error!");
  }
  new_str.value = charp;
  new_str.len = str->len;
  new_str.hash_code = str->hash_code;
  return new_str;
}

static int ustrtb_key_comp(ustring* k1,ustring* k2){
  return ustring_comp(k1,k2);
}

ustring* ustring_table_put(ustring_table* strtb,
			   char*          charp,
			   int            len){
  unsigned int hscd = len > 0 ? udata_hscd(charp,len) : ucharp_hscd(charp);
  int      _len     = len > 0 ? len : strlen(charp);
  ustring  str      = (ustring){charp,_len,hscd};
  ustring* new_str;
  int retval;
  retval = uhstb_ustring_put(strtb,
			     hscd,
			     &str,
			     &new_str,
			     ustrtb_key_put,
			     ustrtb_key_comp);
  if (retval != -1){
    return new_str;
  }else{
    return NULL;
  }
}

static int ustrtb_key_comp_greater(ustring* k1,ustring* k2){
  return 1;
}

ustring* ustring_table_add(ustring_table* strtb,
			   char*          charp,
			   int            len){
  unsigned int hscd = len > 0 ? udata_hscd(charp,len) : ucharp_hscd(charp);
  int      _len     = len > 0 ? len : strlen(charp);
  ustring  str      = (ustring){charp,_len,hscd};
  ustring* new_str;
  int retval;
  retval = uhstb_ustring_put(strtb,
			     hscd,
			     &str,
			     &new_str,
			     ustrtb_key_put,
			     ustrtb_key_comp_greater);
  if (retval != -1){
    return new_str;
  }else{
    return NULL;
  }
}
