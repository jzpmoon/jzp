#include <string.h>
#include "ustring.h"
#include "uhstb_tpl.c"
#include "ustring_table.h"

uhstb_def_tpl(ustring);

static ustring ucall ustrtb_key_put(ustring* key)
{
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

static int ucall ustrtb_key_comp(ustring* k1,ustring* k2)
{
  return ustring_comp(k1,k2);
}

uapi ustring* ucall ustring_table_put(ustring_table* strtb,
			   char*          charp,
			   int            len)
{
  unsigned int hscd = len > 0 ? udata_hscd(charp,len) : ucharp_hscd(charp);
  int      _len     = len > 0 ? len : strlen(charp);
  ustring  str;
  ustring* new_str;
  int retval;

  str.value = charp;
  str.len = _len;
  str.hash_code = hscd;
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

static int ucall ustrtb_key_comp_greater(ustring* k1,ustring* k2)
{
  return 1;
}

uapi ustring* ucall ustring_table_add(ustring_table* strtb,
			   char*          charp,
			   int            len)
{
  unsigned int hscd = len > 0 ? udata_hscd(charp,len) : ucharp_hscd(charp);
  int      _len     = len > 0 ? len : strlen(charp);
  ustring  str;
  ustring* new_str;
  int retval;

  str.value = charp;
  str.len = _len;
  str.hash_code = hscd;
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

static ustring ucall ustrtb_concat_key_put(ustring* key)
{
  ustring** dstr;
  char* charp;
  ustring new_str;
  dstr = (ustring**)key->value;
  charp = ucharp_concat(dstr[0]->value,
			dstr[0]->len,
			dstr[1]->value,
			dstr[1]->len);
  if(!charp){
    uabort("ucharp_new error!");
  }
  new_str.value = charp;
  new_str.len = key->len;
  new_str.hash_code = key->hash_code;
  return new_str;
}

static int ucall ustrtb_concat_key_comp(ustring* k1,ustring* k2)
{
  ustring** dstr;
  int retval;
  if (k1->len > k2->len) {
    return 1;
  } if (k1->len < k2->len) {
    return -1;
  } else {
    dstr = (ustring**)k1->value;
    retval = memcmp(dstr[0]->value,k2->value,dstr[0]->len);
    if (!retval) {
      retval = memcmp(dstr[1]->value,
		      k2->value + dstr[0]->len,
		      dstr[1]->len);
      return retval;
    } else {
      return retval;
    }
  }
}

uapi ustring* ucall ustring_concat_by_strtb(ustring_table* strtb,
				 ustring* str1,
				 ustring* str2)
{
  ustring* new_str;
  int retval;
  ustring str;
  ustring* dstr[2];
  unsigned int hscd;
  int i;
  hscd = str1->hash_code;
  for (i = 0;i < str2->len;i++) {
    hscd *= 31;
  }
  hscd += str2->hash_code;
  str.hash_code = hscd;
  str.len = str1->len + str2->len;
  dstr[0] = str1;
  dstr[1] = str2;
  str.value = (char*)&dstr;

  retval = uhstb_ustring_put(strtb,
			     hscd,
			     &str,
			     &new_str,
			     ustrtb_concat_key_put,
			     ustrtb_concat_key_comp);
  if (retval != -1){
    return new_str;
  }else{
    return NULL;
  }
}
