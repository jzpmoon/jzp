#include "lobj.h"
#include "leval.h"

static leval eval_instance;

void lstartup(){
  URI_DEFINE;
  vgc_heap* heap;
  vcontext* ctx;
  
  heap = vgc_heap_new(1024,
		      1024*10,
		      1024);
  if(!heap){
    uabort("new heap error!");
  }

  ctx = vcontext_new(heap);
  if(!ctx){
    uabort("new context error!");
  }
}

void leval(char* file_path){

}
