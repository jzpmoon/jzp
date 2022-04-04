#ifndef _USTRING_H_
#define _USTRING_H_

#include <string.h>
#include "umacro.h"
#include "udef.h"
#include "ualloc.h"

typedef struct _ustring{
	char* value;
	int len;
	unsigned int hash_code;
} ustring,* ustringp;

uapi int ucall ustring_comp(ustring* s1,ustring* s2);
uapi int ucall ustring_charp_comp(ustring* str,char* charp);
uapi char* ucall ucharp_new(uallocator* alloc,void* data,int len);
uapi ustring* ucall ustring_new(uallocator* alloc,void* value,int len,int hscd);
uapi ustring* ucall ustring_new_by_charp(uallocator* alloc,char* charp);
uapi unsigned int ucall udata_hscd(void* v,int len);
uapi unsigned int ucall ucharp_hscd(char* v);
uapi void ucall ustring_dest(ustring* str);
uapi int ucall ustring_to_integer(ustring* str);
uapi double ucall ustring_to_number(ustring* str);
uapi char* ucall ucharp_concat(char* c1,int len1,char* c2,int len2);
uapi ustring* ucall ustring_concat(uallocator* alloc,ustring* str1,ustring* str2);
uapi ustring* ucall ustring_concatx(uallocator* alloc,ustring* str1,ustring* str2,
							  char* separator);
uapi ustring* ucall usubstring(uallocator* alloc,ustring* str,int pos,int len);
uapi int ucall ustring_char_at(ustring* str,char c,int t);
uapi void ucall uarrev(uui8* arr,usize_t len);
uapi void ucall ustring_log(ustring* str);

#define ustrcmp(cs,ct) strcmp(cs,ct)

#endif
