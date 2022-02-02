#include <string.h>
#include "ustring.h"
#include "uhstb_tpl.c"
#include "ustring_table.h"

uhstb_def_tpl(ustring);

static void* alloc_impl(uallocator* allocator,ualloc_size_t size)
{
  return NULL;
}

static void* allocx_impl(uallocator* allocator,char* data,ualloc_size_t size);

static void free_impl(uallocator* allocator,void* ptr){}

static void clean_impl(uallocator* allocator){}

ustring_table* ustring_table_new(int len)
{
  uallocator* alloc;
  ustring_table* strtb;

  alloc = &u_global_allocator;
  strtb = alloc->alloc(alloc,sizeof(ustring_table));
  if (strtb) {
    strtb->strtb = uhstb_ustring_new(len);
    if (!strtb->strtb) {
      alloc->free(alloc,strtb);
      return NULL;
    }
    strtb->allocator.alloc = alloc_impl;
    strtb->allocator.allocx = allocx_impl;
    strtb->allocator.free = free_impl;
    strtb->allocator.clean = clean_impl;
  }
  return strtb;
}

static ustring ustrtb_key_put(ustring* key){
  ustring* str = key;
  char* charp;
  ustring new_str;
  charp = ucharp_new(str->value,
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
  retval = uhstb_ustring_put(strtb->strtb,
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
  retval = uhstb_ustring_put(strtb->strtb,
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

static void* allocx_impl(uallocator* allocator,char* data,ualloc_size_t size)
{
  ustring_table* strtb;
  ustring* str;

  strtb = (ustring_table*)allocator;
  str = ustring_table_put(strtb,
			  data,
			  size);
  return str;
}
