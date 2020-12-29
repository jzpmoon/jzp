#ifndef _LPARSER_H_
#define _LPARSER_H_

#include "vpass.h"
#include "ltoken.h"

#define LASTHEADER \
  int t

typedef struct _last_obj{
  LASTHEADER;
} last_obj;

last_obj* lparser_parse(ltoken_state* ts);

typedef vps_t*(*last_attr_ft)(vps_mod* vps,
			      vps_dfg* parent,
			      last_obj* ast_obj);

typedef struct _last_attr{
  char* sname;
  ustring* name;
  last_attr_ft action;
} last_attr;

enum {
  lastk_cons,
  lastk_symbol,
  lastk_number,
  lastk_string,
};

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

typedef struct _last_number{
  LASTHEADER;
  ustring* name;
  double dnum;
} last_number;

typedef struct _last_string{
  LASTHEADER;
  ustring* string;
} last_string;

uhstb_decl_tpl(last_attr);

last_cons* last_cons_new(ltoken_state* ts,last_obj* car,last_obj* cdr);

last_symbol* last_symbol_new(ltoken_state* ts,ustring* name,last_attr* attr);

last_number* last_number_new(ltoken_state* ts,ustring* name,double dnum);

last_string* last_string_new(ltoken_state* ts,ustring* string);

void last_obj_log(last_obj* ast_obj);

#define last_car(cons) \
  ((last_cons*)cons)->car

#define last_cdr(cons) \
  ((last_cons*)cons)->cdr

#define last_cdar(cons) \
  last_car(last_cdr(cons))

#define LDEFATTR(aname,sname,body)			\
  vps_t*  _last_attr_action_##aname(vps_mod* vps,	\
				    vps_dfg* parent,	\
				    last_obj* ast_obj){	\
    body						\
      }							\
  static last_attr _last_attr_infor_##aname =		\
    {sname,NULL,_last_attr_action_##aname};

#define LATTRONLOAD(afname,body)			\
  void _lattr_file_init_##afname(ltoken_state* ts){	\
    body						\
  }

#define LDECLATTR(aname) LATTR_INIT(ts,aname)

#define LATTR_INIT(ts,aname)						\
  do{									\
    ustring* str = ustring_table_put(ts->symtb,				\
				     _last_attr_infor_##aname.sname,	\
				     -1);				\
    if(!str){uabort("init attr error!");}				\
    _last_attr_infor_##aname.name = str;				\
    uhstb_last_attr_put(ts->attrtb,					\
			str->hash_code,					\
			&_last_attr_infor_##aname,			\
			NULL,						\
			NULL,						\
			last_attr_put_comp);				\
  }while(0)

int last_attr_put_comp(last_attr* k1,last_attr* k2);

int last_attr_get_comp(last_attr* k1,last_attr* k2);

void ltoken_state_attr_init(ltoken_state* ts);

int last2vps(ltoken_state* ts,last_obj* ast_obj,vps_mod* mod);

#endif
