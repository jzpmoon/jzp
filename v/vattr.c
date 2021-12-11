#include "autogen.h"
#include "_vtemp.attr"

void vattr_init(vreader* reader){
  if (virtb_load()) {
    uabort("ir load error!");
  }
  vattr_file_concat_init(reader);
}
