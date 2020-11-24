#include "vm.h"
#include "lparser.h"

int last_attr_put_comp(last_attr* k1,last_attr* k2){
  return ustring_comp(k1->name,k2->name);
}

int last_attr_get_comp(last_attr* k1,last_attr* k2){
  return ustring_comp(k1->name,k2->name);
}

#include "_ltemp.attr"

void ltoken_state_attr_init(ltoken_state* ts){
  lattr_file_concat_init(ts);
}
