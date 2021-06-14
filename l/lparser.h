#ifndef _LPARSER_H_
#define _LPARSER_H_

#include "lreader.h"

enum lastk{
  lastk_cons,
  lastk_symbol,
  lastk_integer,
  lastk_number,
  lastk_string,
  lastk_character,
  lastk_nil,
};

enum ltk{
  ltk_bad,
  ltk_identify,
  ltk_left,
  ltk_right,
  ltk_string,
  ltk_character,
  ltk_integer,
  ltk_number,
  ltk_nil,
  ltk_eof,
};

extern last_attr last_attr_symcall;

last_obj* lparser_parse(ltoken_state* ts);

typedef struct _last_cons{
  LASTHEADER;
  last_obj* car;
  last_obj* cdr;
} last_cons;

typedef struct _last_symbol{
  LASTHEADER;
  ustring* name;
  last_attr* attr;
} last_symbol;

typedef struct _last_integer{
  LASTHEADER;
  ustring* name;
  int value;
} last_integer;

typedef struct _last_number{
  LASTHEADER;
  ustring* name;
  double value;
} last_number;

typedef struct _last_string{
  LASTHEADER;
  ustring* value;
} last_string;

typedef struct _last_character{
  LASTHEADER;
  int value;
} last_character;

typedef struct _last_nil{
  LASTHEADER;
} last_nil;

last_cons* last_cons_new(ltoken_state* ts,last_obj* car,last_obj* cdr);

last_symbol* last_symbol_new(ltoken_state* ts,ustring* name,last_attr* attr);

last_integer* last_integer_new(ltoken_state* ts,ustring* name,int inte);

last_number* last_number_new(ltoken_state* ts,ustring* name,double dnum);

last_string* last_string_new(ltoken_state* ts,ustring* string);

last_character* last_character_new(ltoken_state* ts,int character);

last_nil* last_nil_new(ltoken_state* ts);

void last_obj_log(last_obj* ast_obj);

#define last_car(cons) \
  ((last_cons*)cons)->car

#define last_cdr(cons) \
  ((last_cons*)cons)->cdr

#define last_cdar(cons) \
  last_car(last_cdr(cons))

#define LDEFATTR(aname,sname,body)			\
  int							\
  _last_attr_action_##aname(last_attr_req* req,		\
			    last_attr_res* res){	\
    body						\
      }							\
  static last_attr _last_attr_infor_##aname =		\
    {sname,NULL,_last_attr_action_##aname};

#define LATTRONLOAD(afname,body)			\
  void _lattr_file_init_##afname(ltoken_state* ts){	\
    body						\
      }

#define LATTR_CONTEXT_FILE(parent)					\
  (parent->t == vcfgk_grp ||						\
   ((vcfg_graph*)parent)->scope == VPS_SCOPE_ENTRY)

#define LATTR_CONTEXT_BLOCK(parent)		\
  (parent->t == vcfgk_blk)

#define LATTR_RETURN(type,vps)			\
  do {						\
    res->res_type = type;			\
    res->res_vps = (vps_t*)(vps);		\
    return 1;					\
  } while(0)

#define LATTR_RETURN_VOID return (0)

#define LDECLATTR(aname) LATTR_INIT(ts,aname)

#define LATTR_INIT(ts,aname)				\
  do{							\
    _last_attr_infor_##aname.name =			\
      ustring_table_put(ts->symtb,			\
			_last_attr_infor_##aname.sname,	\
			-1);				\
    if(!_last_attr_infor_##aname.name){			\
      uabort("init attr error!");			\
    }							\
    uhstb_last_attr_put(ts->attrtb,			\
			_last_attr_infor_##aname.	\
			name->				\
			hash_code,			\
			&_last_attr_infor_##aname,	\
			NULL,				\
			NULL,				\
			last_attr_put_comp);		\
  }while(0)

int last_attr_put_comp(last_attr* k1,last_attr* k2);

int last_attr_get_comp(last_attr* k1,last_attr* k2);

void ltoken_state_attr_init(ltoken_state* ts);

int last2vps(last_attr_req* req,last_attr_res* res);

UDECLFUN(UFNAME lfile2vps,
	 UARGS (lreader* reader,char* file_path,vps_cntr* vps),
	 URET vps_mod*)

#endif
