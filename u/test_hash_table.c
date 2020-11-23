#include <stdio.h>
#include <stddef.h>
#include "ustring.h"
#include "uhash_table.h"
#include "ustring_table.h"

uhash_table hstb[17];
void* putk(void* k){
  return k;
}
int comp(void* k1,void*k2){
	return k1-k2;
}

int hash_table_test(){
	char* a[]={"fds","sdf","qwe","zxc","asd"};
	int i=0,j=0;
	while(i<5){
		uhash_table_put(
		hstb,
		ucharp_hscd(a[i])%17,
		a[i],
		putk,
		comp);
		i++;
	}
	while(j<17){
		printf("%d : ",j);
		uhash_node* n=hstb[j++];
		while(n!=NULL){
			printf("%s -> ",(char*)n->k);
			n=n->next;
		}
		printf("\n");
	}
	return 0;
}

int int_comp(int k1,int k2){
  return k1 - k2;
}

int hstb_test(){
  uhstb_int* hstb = uhstb_int_new(17);
  uiterator iterator;
  int a[]={1,2,3,4,5};
  int i=0;
  while(i<5){
    uhstb_int_put(hstb,
		  a[i],
		  a[i],
		  NULL,
		  int_comp);
    i++;
  }
  (hstb->iterate)(&iterator);
  while(1){
    int* val = (hstb->next)((uset*)hstb,&iterator);
    if (!val){
      break;
    }
    printf("%d \n",*val);
  }
  return 0;
}

int string_table_test(){
  ustring_table* strtb = ustring_table_new(10);
  ustring* str = ustring_table_put(strtb,
				   10,
				   NULL,
				   -1);
  return 0;
}

int main(){
  string_table_test();
}
