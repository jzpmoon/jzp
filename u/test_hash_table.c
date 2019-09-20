#include <stdio.h>
#include <stddef.h>
#include "ustring.h"
#include "uhash_table.h"

uhash_table hstb[17];
void* putk(void* k){
  return k;
}
int comp(void* k1,void*k2){
	return k1-k2;
}

int main(){
	char* a[]={"fds","sdf","qwe","zxc","asd"};
	int i=0,j=0;
	while(i<5){
		uhash_table_put(
		hstb,
		string_hscd(a[i])%17,
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
