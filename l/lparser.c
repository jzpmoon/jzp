#include "uerror.h"
#include "ualloc.h"
#include "lparser.h"
#include "lobj.h"

#define LTOKEN_BUFF_SIZE 100

static ustring* ltoken_state_symbol_finish(ltoken_state* ts){
  ubuffer* buff = ts->buff;
  ustring* str;
  ubuffer_ready_read(buff);
  str = ustring_table_put(ts->symtb,ts->symtb_size,buff->data,buff->limit);
  ubuffer_ready_write(buff);
  return str;
}

static ustring* ltoken_state_string_finish(ltoken_state* ts){
  ubuffer* buff = ts->buff;
  ustring* str;
  ubuffer_ready_read(buff);
  str = ustring_table_add(ts->strtb,ts->strtb_size,buff->data,buff->limit);
  ubuffer_ready_write(buff);
  return str;
}

static void* last_attr_key_put(void* key){
  return key;
}

static void* last_attr_key_get(void* key){
  return key;
}

static int last_attr_key_comp(void* k1,void* k2){
  last_attr* attr1 = (last_attr*)k1;
  last_attr* attr2 = (last_attr*)k2;
  return ustring_comp(attr1->name,attr2->name);
}

int ltoken_next_char(ltoken_state* ts){
  URI_DEFINE;
  int c;
  c = ustream_read_next(ts->stream,URI_REF);
  URI_ERROR;
    URI_CASE(UERR_EOF);
      return c;
    URI_END;
    uabort(URI_DESC);
  URI_END;
  ts->coord.x++;
  return c;
}

int ltoken_look_ahead(ltoken_state* ts){
  URI_DEFINE;
  int c;
  c = ustream_look_ahead(ts->stream,URI_REF);
  URI_ERROR;
    URI_CASE(UERR_EOF);
      return c;
    URI_END;
    uabort(URI_DESC);
  URI_END;
  return c;
}

#define ltoken_new_line(ts)			\
  ((ts)->coord.x=0,(ts)->coord.y++)

#define ltoken_mark(ts,c)			\
  if(ubuffer_write_next((ts)->buff,c) == -1){	\
    ubuffer_empty((ts)->buff);			\
    return (ts)->token = ltk_bad;		\
  }

void ltoken_skip_blank(ltoken_state* ts){
  int c;
  while(1){
    c = ltoken_look_ahead(ts);
    if(c == ' ' || c == '\t' || c== '\r'){
      ltoken_next_char(ts);
      continue;
    }if(c== '\n'){
      ltoken_next_char(ts);
      ltoken_new_line(ts);
      continue;
    }else{
      break;
    }
  }
}

int ltoken_lex_string(ltoken_state* ts){
  ustring* str;
  int      c;
  while(1){
    c = ltoken_next_char(ts);
    if(c == '"'){
      break;
    }
    if(c == LEOF){
      return ts->token = ltk_bad;
    }
    ltoken_mark(ts,c);
  }
  str = ltoken_state_string_finish(ts);
  if(!str){
    uabort("lparse: vstrtb add error!");
  }
  ts->str = str;
  return ts->token = ltk_string;
}

int ltoken_lex_number(ltoken_state* ts){
  ustring* str;
  int      dot = 0;
  int      c;
  while(1){
    c = ltoken_look_ahead(ts);
    if(c == '.'){
      dot++;
    }
    if(dot > 1){
      return ts->token = ltk_bad;
    }
    if(c < '0' || c > '9'){
      break;
    }
    ltoken_mark(ts,c);
    ltoken_next_char(ts);
  }
  str = ltoken_state_symbol_finish(ts);
  if(!str){
    uabort("lparse: vstrtb put error!");
  }
  ts->sym = str;
  ts->num = ustring_to_number(str);
  return ts->token = ltk_number;
}

int ltoken_lex_identify(ltoken_state* ts){
  ustring* str;
  int      c;
  while(1){
    c = ltoken_look_ahead(ts);
    switch(c){
    case '(':
    case ')':
    case '\'':
    case '"':
    case ' ':
    case '\n':
    case '\t':
    case '\r':
    case LEOF:
      str = ltoken_state_symbol_finish(ts);
      if(!str){
	uabort("lparse: strtb put error!");
      }
      ts->sym = str;
      return ts->token = ltk_identify;
    default:
      ltoken_mark(ts,c);
      ltoken_next_char(ts);
      continue;
    }
  }
}

int ltoken_next(ltoken_state* ts){
  int c;
  ltoken_skip_blank(ts);
  c = ltoken_next_char(ts);
  switch(c){
  case '(':
    return ts->token = ltk_left;
  case ')':
    return ts->token = ltk_right;
  case '\'':
    return ts->token = ltk_quote;
  case '"':
    return ltoken_lex_string(ts);
  case '+':
  case '-':
    ltoken_mark(ts,c);
    c = ltoken_look_ahead(ts);
    if(c < '0' || c > '9'){
      return ltoken_lex_identify(ts);
    }else{
      goto lab_num;
    }
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    ltoken_mark(ts,c);
  lab_num:
    return ltoken_lex_number(ts);
  case LEOF:
    return ts->token = ltk_eof;
  default:
    ltoken_mark(ts,c);
    return ltoken_lex_identify(ts);
  }
}

last_obj* lparser_atom_parse(ltoken_state* ts){
  int tk = ts->token;
  switch(tk){
  case ltk_string:
    {
      last_string* str = last_string_new(ts->str);
      if(!str){
	uabort("lparser_atom_parse error!");
      }
      return (last_obj*)str;
    }
  case ltk_identify:
    {
      last_attr* attr;
      last_symbol* sym;
      attr = uhash_table_get(ts->attrtb,
			     ts->sym->hash_code % ts->attrtb_size,
			     ts->sym,
			     last_attr_key_get,
			     last_attr_key_comp);
      sym = last_symbol_new(ts->sym,attr);
      if(!sym){
	uabort("lparser_atom_parse error!");
      }
      return (last_obj*)sym;
    }
  case ltk_number:
    {
      last_number* num = last_number_new(ts->num);
      if(!num){
	uabort("lparser_atom_parse error!");
      }
      return  (last_obj*)num;
    }
  default:
    ltoken_log(ts);
    uabort("lparser_atom_parse error!");
  }
}

last_obj* lparser_exp_parse(ltoken_state* ts){
  last_obj* s_exp = NULL;
  last_cons* cons = NULL;
  while(1){
    last_obj* car;
    int   tk = ltoken_next(ts);
    switch(tk){
    case ltk_left:
      car = lparser_exp_parse(ts);
      break;
    case ltk_right:
      return s_exp;
    default:
      car = lparser_atom_parse(ts);
    }
    {
      last_obj* tmp = (last_obj*)last_cons_new(NULL,NULL);
      if(!tmp){
	uabort("lparser_parse error!");
      }
      if(!cons){
	s_exp = tmp;
      }else{
	cons->cdr = tmp;
      }
      cons = (last_cons*)tmp;
      cons->car = car;
    }
  }
}

/*
 * s-exp := ( exp ) | atom
 * exp   := exp exp | s-exp | atom
*/
last_obj* lparser_parse(ltoken_state* ts){
  last_obj* s_exp;
  while(1){
    int tk = ltoken_next(ts);
    switch(tk){
    case ltk_left:
      s_exp = lparser_exp_parse(ts);
      return s_exp;
    default:
      s_exp = lparser_atom_parse(ts);
      return s_exp;
    }
  }
}

void ltoken_log(ltoken_state* ts){
  char* sym;
  char* str;
  sym = ts->sym->value;
  str = ts->str->value;
  ulog ("********token state begin");
  ulog1("token  :%d",ts->token);
  ulog1("symbol :%s",sym);
  ulog1("string :%s",str);
  ulog1("number :%f",ts->num);
  ulog ("********token state   end");
}

void lparser_atom_log(last_obj* s_exp){
  if(s_exp){
    if(s_exp->t == lastk_symbol){
      last_symbol* sym = (last_symbol*)s_exp;
      ulog1("  %s",sym->name->value);
    }else if(s_exp->t == lastk_string){
      last_string* str = (last_string*)s_exp;
      ulog1("  \"%s\"",str->string->value);
    }else if(s_exp->t == lastk_number){
      last_number* num = (last_number*)s_exp;
      ulog1("  %f",num->dnum);
    }else{
      ulog(" [unkonw]");
    }
  }else{
    ulog(" ()");
  }
}

void last_obj_log(last_obj* ast_obj){
  last_obj* s_exp = ast_obj;
  last_cons* cons;
  if(s_exp){
    if(s_exp->t == lastk_cons){
      cons = (last_cons*)s_exp;
      ulog("(");
      while(1){
	last_obj* cdr;
	last_obj_log(cons->car);
	cdr = cons->cdr;
	if(cdr){
	  if(cdr->t == lastk_cons){
	    cons = (last_cons*)cdr;
	    continue;
	  }else{
	    lparser_atom_log(cdr);
	    break;
	  }
	}else{
	  ulog(")");
	  break;
	}
      }
      return;
    }
  }
  lparser_atom_log(s_exp);
}

last_cons* last_cons_new(last_obj* car,last_obj* cdr){
  last_cons* cons = ualloc(sizeof(last_cons));
  if(cons){
    cons->t = lastk_cons;
    cons->car = car;
    cons->cdr = cdr;
  }
  return cons;
}

last_symbol* last_symbol_new(ustring* name,last_attr* attr){
  last_symbol* symbol = ualloc(sizeof(last_symbol));
  if(symbol){
    symbol->t = lastk_symbol;
    symbol->name = name;
    symbol->attr = attr;
  }
  return symbol;
}

last_number* last_number_new(double dnum){
  last_number* number = ualloc(sizeof(last_number));
  if(number){
    number->t = lastk_number;
    number->dnum = dnum;
  }
  return number;
}

last_string* last_string_new(ustring* string){
  last_string* str = ualloc(sizeof(last_string));
  if(str){
    str->t = lastk_string;
    str->string = string;
  }
  return str;
}

LDEFATTR(subr,"subr",{
    
    return NULL;
})

void ltoken_state_attr_init(ltoken_state* ts){
  LATTR_INIT(ts,subr);
}

void ltoken_state_init(ltoken_state* ts,
		       ustream* stream){
  ts->stream = stream;
  ubuffer_ready_write(ts->buff);
  ts->token = ltk_bad;
  ts->str = NULL;
  ts->sym = NULL;
  ts->num = 0;
  ts->bool = 0;
  ts->coord.x = 1;
  ts->coord.y = 1;
}

#define LATTR_TABLE_SIZE 17

ltoken_state* ltoken_state_new(ustream* stream,
			       ustring_table* symtb,
			       int symtb_size,
			       ustring_table* strtb,
			       int strtb_size){
  ltoken_state* ts;
  ubuffer* buff;
  uhash_table* attrtb;
  
  buff = ubuffer_new(LTOKEN_BUFF_SIZE);
  if(!buff){
    goto err;
  }
  
  attrtb = uhash_table_new(LATTR_TABLE_SIZE);
  if(!attrtb){
    goto err;
  }

  unew(ts,sizeof(ltoken_state),goto err;);
  
  ts->buff = buff;
  ts->symtb = symtb;
  ts->symtb_size = symtb_size;
  ts->strtb = strtb;
  ts->strtb_size = strtb_size;
  ts->attrtb = attrtb;
  ts->attrtb_size = LATTR_TABLE_SIZE;
  
  ltoken_state_init(ts,stream);
  ltoken_state_attr_init(ts);
  
  return ts;
 err:
  ubuffer_dest(buff);
  return NULL;
}

vdfg* last2dfg(ltoken_state* ts,last_obj* ast_obj){
  switch(ast_obj->t){
  case lastk_cons:{
    last_obj* obj = last_car((last_cons*)ast_obj);
    if(obj->t == lastk_symbol){
      last_symbol* sym = (last_symbol*)obj;
      if(sym->attr){
	last_attr* attr = sym->attr;
	return (attr->action)(ast_obj);
      }
    }
  }
    break;
  case lastk_symbol:
    break;
  case lastk_number:
    break;
  case lastk_string:
    break;
  default:
    uabort("ast type error!");
  }
  return NULL;
}
