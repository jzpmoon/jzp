#include <stdio.h>
#include "vgc.h"

int main(){
  VgcArray arr;
  VgcNum num;
  VgcString str;
  arr.t=gc_arr;
  arr.len=1;
  num.t=gc_num;
  str.t=gc_str;
  printf("%d\n",vgcObjSize((VgcObj*)&arr));
  printf("%d\n",vgcObjSize((VgcObj*)&num));
  printf("%d\n",vgcObjSize((VgcObj*)&str));
}
