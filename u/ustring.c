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
	if(s1->len==s2->len){
		return memcmp(
			s1->value,s2->value,s1->len);
	}else if(s1->len<s2->len){
		return -1;
	}else{
		return 1;
	}
}

double ustring_to_number(ustring* str){
  return atof(str->value);
}

void ustring_log(ustring* str){
  ulog1("value:%s",str->value);
  ulog1("len  :%d",str->len);
  ulog1("hscd :%d",str->hash_code);
}
