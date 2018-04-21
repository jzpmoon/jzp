#include <malloc.h>
#include <stddef.h>
#include <string.h>
#include "uassert.h"
#include "ustring.h"

unsigned int data_hscd(void* v,int len){
	unsigned int code=0,i=0;
	while(i<len){
		code=code*31+((char*)v)[i++];
	}
	return code;
}

unsigned int string_hscd(char* v){
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

char* utf8_from_gbk(char* gbk){
  return NULL;
}
