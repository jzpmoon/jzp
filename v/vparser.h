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
  vastk_keyword,
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
  vtk_keyword,
  vtk_eof,
};

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

typedef struct _vast_keyword{
  VASTHEADER;
  vast_kw kw;
} vast_keyword;

vast_cons* vast_cons_new(vtoken_state* ts,vast_obj* car,vast_obj* cdr);

vast_symbol* vast_symbol_new(vtoken_state* ts,ustring* name,vast_attr* attr);

vast_integer* vast_integer_new(vtoken_state* ts,ustring* name,int inte);

vast_number* vast_number_new(vtoken_state* ts,ustring* name,double dnum);

vast_string* vast_string_new(vtoken_state* ts,ustring* string);

vast_character* vast_character_new(vtoken_state* ts,int character);

vast_keyword* vast_keyword_new(vtoken_state* ts);

void vast_obj_log(vast_obj* ast_obj);

vast_obj* vast_car(vast_obj* cons);

vast_obj* vast_cdr(vast_obj* cons);

vast_obj* vast_cdar(vast_obj* cons);

#define vast_typeof(obj,type) (!(obj) || (obj)->t != type)

#define vast_consp(obj) vast_typeof(obj,vastk_cons)

#define vast_symbolp(obj) vast_typeof(obj,vastk_symbol)

#define vast_integerp(obj) vast_typeof(obj,vastk_integer)

#define vast_numberp(obj) vast_typeof(obj,vastk_number)

#define vast_stringp(obj) vast_typeof(obj,vastk_string)

#define vast_characterp(obj) vast_typeof(obj,vastk_character)

#define vast_keywordp(obj) vast_typeof(obj,vastk_keyword)

#define VDEFATTR(aname,sname,body)			 \
  UDEFUN(UFNAME _vast_attr_action_##aname,		 \
	 UARGS (vast_attr_req* req,vast_attr_res* res),	 \
	 URET int)					 \
  UDECLARE						 \
      vast_attr_ft this_fun = _vast_attr_action_##aname; \
  UBEGIN						 \
    body						 \
  UEND							 \
  static vast_attr _vast_attr_infor_##aname =		 \
    {sname,NULL,_vast_attr_action_##aname};

#define VATTRONLOAD(afname,body)			\
  UDEFUN(UFNAME _vattr_file_init_##afname,		\
	 UARGS (vreader* reader),			\
	 URET void)					\
    UDECLARE						\
    UBEGIN						\
      body						\
    UEND

#define VATTR_RETURN(type,obj)			\
  do {						\
    res->res_from = this_fun;			\
    res->res_type = type;			\
    res->res_obj = obj;				\
    return 1;					\
  } while(0)

#define VATTR_RETURN_VOID			\
  do {						\
    res->res_from = this_fun;			\
    return (0);					\
  } while(0)

#define vast_res_from(from)			\
  (res->res_from == _vast_attr_action_##from)

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

UDECLFUN(UFNAME vfile2obj,
	 UARGS (vreader* reader,ustring* file_path,vast_attr_req* req,
		vast_attr_res* res),
	 URET int);
#endif
