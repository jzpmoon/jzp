#ifndef _USTRING_H_
#define _USTRING_H_

typedef struct _ustring{
	char* value;
	int len;
	unsigned int hash_code;
} ustring,* ustringp;

int ustring_comp(ustring*s1,ustring* s2);
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
void ustring_log(ustring* str);

#endif
