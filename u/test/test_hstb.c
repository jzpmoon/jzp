#include <stdio.h>
#include <stddef.h>
#include "ustring.h"
#include "uhstb.h"
#include "ustring_table.h"


int int_comp(int* k1,int* k2){
  return k1 - k2;
}

int hstb_test(){
  uhstb_int* hstb = uhstb_int_new(17);
  ucursor cursor;
  int a[]={1,2,3,4,5};
  int i=0;
  while(i<5){
    uhstb_int_put(hstb,
		  a[i],
		  &a[i],
		  NULL,
		  NULL,
		  int_comp);
    i++;
  }
  (hstb->iterate)(&cursor);
  while(1){
    int* val = (hstb->next)((uset*)hstb,&cursor);
    if (!val){
      break;
    }
    printf("%d \n",*val);
  }
  return 0;
}

int string_table_test(){
  char* charp = "hello";
  ustring_table* strtb = ustring_table_new(10);
  ustring* str = ustring_table_put(strtb,
				   charp,
				   -1);
  return 0;
}

int main(){
  string_table_test();
  hstb_test();
}
