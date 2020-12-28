#include <stdio.h>
#include "udbuffer.h"
#include "ustream.h"
#include "ustring_table.h"


int main_test(){
  URI_DEFINE;
  FILE* file;
  ustream* stream;
  int next;
  
  file = fopen("./test.txt","r");
  if(!file){
    uabort("file open error");
  }
  
  stream = ustream_new_by_file(USTREAM_INPUT,file,URI_REF);
  URI_ERROR;
  uabort(URI_DESC);
  URI_END;
  
  do{
    next = ustream_read_next(stream,URI_REF);
    URI_ERROR
      URI_CASE(UERR_EOF)
        break;  
      URI_ELSE
        uabort(URI_DESC);
      URI_END
    URI_END
    printf("read %c\n",next);
  } while(next != -1);
  
  return 0;
}

int test_buffer(){
  FILE* file;
  udbuffer dbuff = {NULL,NULL,NULL,10};
  int count;
  int next;
  
  file = fopen("./test.txt","r");
  if(!file){
    uabort("file open error");
  }

  do{
    next = udbuffer_read_next(&dbuff);
    if(next == -1){
      count = udbuffer_read_from_file(&dbuff,file);
      printf("count %d\n",count);
      next = udbuffer_read_next(&dbuff);
    }
    printf("read %c\n",next);
  } while(next != -1);

  fclose(file);

  return 0;
}

int string_table_test(){
  char* a = "hello";
  ustring_table* strtb = ustring_table_new(10);
  ulog("1");
  ustring* str = ustring_table_put(strtb,
				   a,
				   -1);
  
  ulog("2");
  return 0;
}

int main(){
  main_test();
}
