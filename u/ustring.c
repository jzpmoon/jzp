#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "uerror.h"
#include "ualloc.h"
#include "ustring.h"

unsigned int udata_hscd(void* v,int len){
	unsigned int code=0,i=0;
	while(i<len){
		code=code*31+((char*)v)[i++];
	}
	return code;
}

unsigned int ucharp_hscd(char* v){
	unsigned int code=0,i=0;
	while(v[i]!='\0'){
		code=code*31+v[i++];
	}
	return code;
}

char* ucharp_new(void* data,int len){
  char* charp;
  unew(charp,len+1,return NULL;);
  memcpy(charp,data,len);
  charp[len]='\0';
  return charp;
}

ustring* ustring_new(
	void* value,int len,int hscd){
	ustring* str=NULL;
	char* v=NULL;
	unew(str,sizeof(ustring),goto err;)
        unew(v,len+1,goto err;)
	memcpy(v,value,len);
	v[len]='\0';
	str->hash_code=hscd;
	str->len=len;
	str->value=v;
	return str;
 err:
	free(str);
	return NULL;
}

ustring* ustring_new_by_charp(char* charp){
  unsigned int hscd = ucharp_hscd(charp);
  int len = strlen(charp);
  ustring* string = ustring_new(charp,len,hscd);
  return string;
}

void ustring_dest(ustring* str){
	free(str->value);
	free(str);
}

int ustring_comp(ustring* s1,ustring* s2){
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

int ustring_charp_comp(ustring* str,char* charp)
{
  char* str_val;
  
  str_val = str->value;
  return strcmp(str_val,charp);
}

int ustring_to_integer(ustring* str){
  return atoi(str->value);
}

double ustring_to_number(ustring* str){
  return atof(str->value);
}

ustring* ustring_concat(ustring* str1,ustring* str2)
{
  int len;
  char* charp;
  ustring* str;

  str = ualloc(sizeof(ustring));
  if (!str) {
    return NULL;
  }
  len = str1->len + str2->len;
  charp = ualloc(len + 1);
  if(!charp) {
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

ustring* ustring_concatx(ustring* str1,ustring* str2,char* sep)
{
  int len;
  int sep_len;
  char* charp;
  ustring* str;

  str = ualloc(sizeof(ustring));
  if (!str) {
    return NULL;
  }
  sep_len = strlen(sep);
  len = str1->len + str2->len + sep_len;
  charp = ualloc(len + 1);
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

void ustring_log(ustring* str){
  ulog1("value:%s",str->value);
  ulog1("len  :%d",str->len);
  ulog1("hscd :%d",str->hash_code);
}
