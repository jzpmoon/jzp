#include "lparser.h"

void* last_attr_key_put(void* key){
  return key;
}

void* last_attr_key_get(void* key){
  return key;
}

int last_attr_put_comp(void* k1,void* k2){
  last_attr* attr1 = (last_attr*)k1;
  last_attr* attr2 = (last_attr*)k2;
  return ustring_comp(attr1->name,attr2->name);
}

int last_attr_get_comp(void* k1,void* k2){
  ustring* sym = (ustring*)k1;
  last_attr* attr = (last_attr*)k2;
  return ustring_comp(sym,attr->name);
}

#include "ltemp.attr"

void ltoken_state_attr_init(ltoken_state* ts){
  _lattr_file_concat_init(ts);
}
