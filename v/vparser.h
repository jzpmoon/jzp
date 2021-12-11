#ifndef _VPARSER_H_
#define _VPARSER_H_

#include "vreader.h"

enum vastk{
  vastk_cons,
  vastk_symbol,
  vastk_integer,
  vastk_number,
  vastk_string,
  vastk_character,
  vastk_nil,
};

enum vtk{
  vtk_bad,
  vtk_identify,
  vtk_left,
  vtk_right,
  vtk_string,
  vtk_character,
  vtk_integer,
  vtk_number,
  vtk_nil,
  vtk_eof,
};

extern vast_attr vast_attr_symcall;

vast_obj* vparser_parse(vtoken_state* ts);

typedef struct _vast_cons{
  VASTHEADER;
  vast_obj* car;
  vast_obj* cdr;
} vast_cons;

typedef struct _vast_symbol{
  VASTHEADER;
  ustring* name;
  vast_attr* attr;
} vast_symbol;

typedef struct _vast_integer{
  VASTHEADER;
  ustring* name;
  int value;
} vast_integer;

typedef struct _vast_number{
  VASTHEADER;
  ustring* name;
  double value;
} vast_number;

typedef struct _vast_string{
  VASTHEADER;
  ustring* value;
} vast_string;

typedef struct _vast_character{
  VASTHEADER;
  int value;
} vast_character;

typedef struct _vast_nil{
  VASTHEADER;
} vast_nil;

vast_cons* vast_cons_new(vtoken_state* ts,vast_obj* car,vast_obj* cdr);

vast_symbol* vast_symbol_new(vtoken_state* ts,ustring* name,vast_attr* attr);

vast_integer* vast_integer_new(vtoken_state* ts,ustring* name,int inte);

vast_number* vast_number_new(vtoken_state* ts,ustring* name,double dnum);

vast_string* vast_string_new(vtoken_state* ts,ustring* string);

vast_character* vast_character_new(vtoken_state* ts,int character);

vast_nil* vast_nil_new(vtoken_state* ts);

void vast_obj_log(vast_obj* ast_obj);

#define vast_car(cons) \
  ((vast_cons*)cons)->car

#define vast_cdr(cons) \
  ((vast_cons*)cons)->cdr

#define vast_cdar(cons) \
  vast_car(vast_cdr(cons))

#define VDEFATTR(aname,sname,body)			\
  int							\
  _vast_attr_action_##aname(vast_attr_req* req,		\
			    vast_attr_res* res){	\
    body						\
      }							\
  static vast_attr _vast_attr_infor_##aname =		\
    {sname,NULL,_vast_attr_action_##aname};

#define VATTRONLOAD(afname,body)			\
  void _vattr_file_init_##afname(vreader* reader){	\
    body						\
      }

#define VATTR_RETURN(type,obj)			\
  do {						\
    res->res_type = type;			\
    res->res_obj = obj;				\
    return 1;					\
  } while(0)

#define VATTR_RETURN_VOID return (0)

#define VDECLATTR(aname) VATTR_INIT(reader,aname)

#define VATTR_INIT(reader,aname)			\
  do{							\
    _vast_attr_infor_##aname.name =			\
      ustring_table_put((reader)->symtb,		\
			_vast_attr_infor_##aname.sname,	\
			-1);				\
    if(!_vast_attr_infor_##aname.name){			\
      uabort("init attr error!");			\
    }							\
    uhstb_vast_attr_put((reader)->attrtb,		\
			_vast_attr_infor_##aname.	\
			name->				\
			hash_code,			\
			&_vast_attr_infor_##aname,	\
			NULL,				\
			NULL,				\
			vast_attr_put_comp);		\
  }while(0)

int vast_attr_put_comp(vast_attr* k1,vast_attr* k2);

int vast_attr_get_comp(vast_attr* k1,vast_attr* k2);

int vast2obj(vast_attr_req* req,vast_attr_res* res);

#endif
