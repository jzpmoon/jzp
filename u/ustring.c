#include <stddef.h>
#include <stdlib.h>
#include <ctype.h>
#include "uerror.h"
#include "ualloc.h"
#include "ustring.h"

uapi unsigned int ucall udata_hscd(void* v,int len){
  unsigned int code = 0;
  int i = 0;
  while (i < len) {
    code = code * 31 + ((char*)v)[i++];
  }
  return code;
}

uapi unsigned int ucall ucharp_hscd(char* v){
	unsigned int code=0,i=0;
	while(v[i]!='\0'){
		code=code*31+v[i++];
	}
	return code;
}

uapi char* ucall ucharp_new(uallocator* alloc,void* data,int len){
  uallocator* _alloc;
  char* charp;
  
  _alloc = alloc;
  if (!_alloc) {
    _alloc = &u_global_allocator;
  }
  charp = _alloc->alloc(_alloc,len + 1);
  if (!charp) {
    return NULL;
  }
  memcpy(charp,data,len);
  charp[len]='\0';
  return charp;
}

uapi int ucall ucharp_isint(char* charp)
{
  int i;

  for (i = 0; charp[i] != '\0'; i++) {
    if (!isdigit(charp[i])) {
      return 0;
    }
  }
  return 1;
}

uapi ustring* ucall ustring_new(uallocator* alloc,void* value,int len,int hscd)
{
  uallocator* _alloc;
  ustring* str;
  char* v;

  _alloc = alloc;
  if (!_alloc) {
    _alloc = &u_global_allocator;
  }
  str = _alloc->alloc(_alloc,sizeof(ustring));
  if (!str) {
    goto err;
  }
  v = _alloc->alloc(_alloc,len + 1);
  if (!v) {
    goto err;
  }
  memcpy(v,value,len);
  v[len] = '\0';
  str->hash_code = hscd;
  str->len = len;
  str->value = v;
  return str;
 err:
  _alloc->free(_alloc,str);
  return NULL;
}

uapi ustring* ucall ustring_new_by_charp(uallocator* alloc,char* charp)
{
  uallocator* _alloc;
  unsigned int hscd;
  int len;
  ustring* string;

  _alloc = alloc;
  if (!_alloc) {
    _alloc = &u_global_allocator;
  }
  hscd = ucharp_hscd(charp);
  len = strlen(charp);
  string = ustring_new(_alloc,charp,len,hscd);
  return string;
}

uapi ustring ucall ustring_init(char* charp)
{
  ustring str;

  str.hash_code = ucharp_hscd(charp);
  str.len = strlen(charp);
  str.value = charp;

  return str;
}

uapi void ucall ustring_dest(ustring* str){
	free(str->value);
	free(str);
}

uapi int ucall ustring_comp(ustring* s1,ustring* s2){
  if(s1 == s2){
    return 0;
  }else if(s1->len == s2->len){
    if(s1->value == s2->value){
      return 0;
    }
    return memcmp(s1->value,s2->value,s1->len);
  }else if(s1->len < s2->len){
    return -1;
  }else{
    return 1;
  }
}

uapi int ucall ustring_charp_comp(ustring* str,char* charp)
{
  char* str_val;
  
  str_val = str->value;
  return strcmp(str_val,charp);
}

uapi int ucall ustring_to_integer(ustring* str){
  return (int)strtol(str->value,NULL,0);
}

uapi double ucall ustring_to_number(ustring* str){
  return atof(str->value);
}

uapi char* ucall ucharp_concat(char* c1,int len1,char* c2,int len2)
{
  int len = len1 + len2;
  char* c = ualloc(len + 1);
  if (!c) {
    return NULL;
  }
  memcpy(c,c1,len1);
  memcpy(c + len1,c2,len2);
  c[len] = '\0';
  return c;
}

uapi ustring* ucall ustring_concat(uallocator* alloc,ustring* str1,ustring* str2)
{
  uallocator* _alloc;
  int len;
  char* charp;
  ustring* str;

  _alloc = alloc;
  if (!_alloc) {
    _alloc = &u_global_allocator;
  }
  str = _alloc->alloc(_alloc,sizeof(ustring));
  if (!str) {
    return NULL;
  }
  len = str1->len + str2->len;
  charp = _alloc->alloc(_alloc,len + 1);
  if (!charp) {
    ufree(str);
    return NULL;
  }
  memcpy(charp,str1->value,str1->len);
  memcpy(charp + str1->len,str2->value,str2->len);
  charp[len] = '\0';
  str->value = charp;
  str->len = len;
  str->hash_code = ucharp_hscd(charp);
  
  return str;
}

uapi ustring* ucall ustring_concatx(uallocator* alloc,ustring* str1,ustring* str2,
			 char* sep)
{
  uallocator* _alloc;
  int len;
  int sep_len;
  char* charp;
  ustring* str;

  _alloc = alloc;
  if (!_alloc) {
    _alloc = &u_global_allocator;
  }
  str = _alloc->alloc(_alloc,sizeof(ustring));
  if (!str) {
    return NULL;
  }
  sep_len = strlen(sep);
  len = str1->len + str2->len + sep_len;
  charp = _alloc->alloc(_alloc,len + 1);
  if(!charp) {
    ufree(str);
    return NULL;
  }
  memcpy(charp,str1->value,str1->len);
  memcpy(charp + str1->len,sep,sep_len);
  memcpy(charp + sep_len + str1->len,str2->value,str2->len);
  charp[len] = '\0';
  str->value = charp;
  str->len = len;
  str->hash_code = ucharp_hscd(charp);
  
  return str;  
}

uapi ustring* ucall usubstring(uallocator* alloc,ustring* str,int pos,int len)
{
  uallocator* _alloc;
  ustring* substr;
  int sublen;
  unsigned int hscd;

  _alloc = alloc;
  if (!_alloc) {
    _alloc = &u_global_allocator;
  }
  if (len <= 0) {
    return NULL;
  }
  if (str->len - 1 > pos) {
    if (str->len - pos > len) {
      sublen = len;
    } else {
      sublen = str->len - pos;
    }
  } else {
    return NULL;
  }
  hscd = udata_hscd(str->value,sublen);
  substr = ustring_new(_alloc,str->value + pos,sublen,hscd);
  
  return substr;
}

uapi int ucall ustring_char_at(ustring* str,char c,int t)
{
  char* value;
  char tmp;
  int i;
  int j;
  
  value = str->value;
  if (t > 0) {
    i = 0;
    j = 1;
    while (1) {
      tmp = value[i];
      if (tmp == c) {
	if (j == t) {
	  return i;
	} else {
	  j++;
	}
      } else if (tmp == '\0') {
	break;
      }
      i++;
    }
  } else if (t < 0) {
    i = str->len - 1;
    j = -1;
    while (1) {
      if (i == -1) {
	break;
      }
      tmp = value[i];
      if (tmp == c) {
	if (j == t) {
	  return i;
	} else {
	  j--;
	}
      }
      i--;
    }
  }
  return -1;
}

uapi void ucall uarrev(uui8* arr,usize_t len)
{
  usize_t i = 0;
  while (i < len / 2) {
    uui8 tmp = arr[i];
    arr[i] = arr[len - i - 1];
    arr[len - i - 1] = tmp;
    i++;
  }
}

uapi void ucall ustring_log(ustring* str){
  udebug1("value:%s",str->value);
  udebug1("len  :%d",str->len);
  udebug1("hscd :%d",str->hash_code);
}
