#ifndef _USTRING_H_
#define _USTRING_H_

typedef struct _ustring{
	char* value;
	int len;
	unsigned int hash_code;
} ustring;

int ustring_comp(ustring*s1,ustring* s2);
ustring* ustring_new(
	void* value,int len,int hscd);
ustring* ustring_new_by_charp(
	char* charp);
unsigned int data_hscd(void* v,int len);
unsigned int string_hscd(char* v);
void ustring_dest(ustring* str);
double ustring_to_number(ustring* str);
void ustring_log(ustring* str);

#endif
