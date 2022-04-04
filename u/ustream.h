#ifndef _USTREAM_H_
#define _USTREAM_H_

#include "umacro.h"
#include "udbuffer.h"
#include "ustring_table.h"

#define USTREAM_INPUT  0
#define USTREAM_OUTPUT 1

#define USTREAM_BUFF   0
#define USTREAM_FILE   1

typedef struct _ufile_infor ufile_infor;
typedef struct _ustream ustream;

struct _ustream{
  unsigned int iot:1;
  unsigned int dst:1;
  uallocator* allocator;
  union {
    ubuffer* buff;
    struct {
      udbuffer* dbuff;
      FILE*     file;
    } s;
  } u;
};

struct _ufile_infor{
  ustring* file_path;
  ustring* dir_name;
  ustring* file_name;
};

uapi ufile_infor* ucall ufile_init_by_strtb(ustring_table* strtb,
                                      ufile_infor* fi,
				                      ustring* file_path);

uapi void ucall ufile_log(ufile_infor* fi);

uapi ustream* ucall ustream_new_by_buff(int iot,ubuffer* buff,URI_DECL);

uapi ustream* ucall ustream_new_by_file(int iot,ustring* file_path,URI_DECL);

uapi ustream* ucall ustream_new_by_fd(int iot,FILE* fd,URI_DECL);

uapi ustream* ucall ustream_new(int iot,int dst);

uapi ustream* ucall ustream_alloc(uallocator* allocator,int iot,int dst);

uapi void ucall ustream_dest(ustream* stream);

uapi int ucall ustream_open_by_path(ustream* stream,
			 ustring* file_path);

uapi void ucall ustream_close(ustream* stream);

uapi int ucall ustream_read_to_buff(ustream* stream,ubuffer* buff,URI_DECL);

uapi int ucall ustream_read_next(ustream* stream,URI_DECL);

uapi int ucall ustream_look_ahead(ustream* stream,URI_DECL);

uapi int ucall ustream_write_dnum(ustream* stream,double dnum,URI_DECL);

uapi int ucall ustream_write_int(ustream* stream, int inte,URI_DECL);

uapi int ucall ustream_write_char(ustream* stream, int chara,URI_DECL);

uapi int ucall ustream_write_string(ustream* stream,char* charp,URI_DECL);

#endif
