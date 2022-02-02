#ifndef _USTRING_H_
#define _USTRING_H_

#include "udef.h"
#include "ualloc.h"

typedef struct _ustring{
	char* value;
	int len;
	unsigned int hash_code;
} ustring,* ustringp;

int ustring_comp(ustring* s1,ustring* s2);
int ustring_charp_comp(ustring* str,char* charp);
char* ucharp_new(void* data,int len);
ustring* ustring_new(
	void* value,int len,int hscd);
ustring* ustring_new_by_charp(
	char* charp);
unsigned int udata_hscd(void* v,int len);
unsigned int ucharp_hscd(char* v);
void ustring_dest(ustring* str);
int ustring_to_integer(ustring* str);
double ustring_to_number(ustring* str);
ustring* ustring_concat(ustring* str1,ustring* str2);
ustring* ustring_concatx(ustring* str1,ustring* str2,char* separator);
ustring* usubstring(uallocator* alloc,ustring* str,int pos,int len);
int ustring_char_at(ustring* str,char c,int t);
void uarrev(uui8* arr,usize_t len);
void ustring_log(ustring* str);

#endif
