#include "uerror.h"
#include "ltoken.h"

#define ltoken_next_char(ts) \
  *(ts)->pos++;(ts)->coord.x++
#define ltoken_new_line(ts) \
  (ts)->coord.x=0;(ts)->coord.y++
#define ltoken_back_char(ts) \
  (ts)->pos--;(ts)->coord.x--
#define ltoken_look_ahead(ts) \
  *(ts)->pos+1

void ltoken_skip_blank(ltoken_state* ts){
  int c;
  while(1){
    c = ltoken_next_char(ts);
    if(c == ' ' || c == '\t' || c== '\r'){
      continue;
    }if(c== '\n'){
      ltoken_new_line(ts);
      continue;
    }else{
      ltoken_back_char(ts);
      break;
    }
  }
}

int ltoken_lex_string(ltoken_state* ts,vgc_heap* heap){
  vgc_str* str;
  char*    begin;
  int      len;
  int      c;
  begin = ts->pos;
  while(1){
    c = ltoken_next_char(ts);
    if(c == '"'){
      break;
    }
    if(c == LEOF){
      return ts->token = ltk_bad;
    }
  }
  len = ts->pos - begin - 1;
  str = vgc_str_newc(heap,begin,len);
  if(!str){
    uabort("lparse: new vgc_str error!");
  }
  ts->str = str;
  return ts->token = ltk_string;
}

int ltoken_lex_number(ltoken_state* ts){
  ustring* str;
  char*    begin;
  int      len;
  int      dot;
  int      c;
  dot   = 0;
  begin = ts->pos - 1;
  while(1){
    c = ltoken_next_char(ts);
    if(c == '.'){
      dot++;
    }
    if(dot > 1){
      return ts->token = ltk_bad;
    }
    if(c < '0' || c > '9'){
      ltoken_back_char(ts);
      break;
    }
  }
  len = ts->pos - begin;
  str = vstrtb_put(begin,len);
  if(!str){
    uabort("lparse: vstrtb_put error!");
  }
  ts->sym = str;
  ts->num = ustring_to_number(str);
  return ts->token = ltk_number;
}

int ltoken_lex_identify(ltoken_state* ts){
  ustring* str;
  char*    begin;
  int      len;
  int      c;
  begin = ts->pos - 1;
  while(1){
    c = ltoken_next_char(ts);
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
      ltoken_back_char(ts);
      len = ts->pos - begin;
      str = vstrtb_put(begin,len);
      if(!str){
	uabort("lparse: vstrtb_put error!");
      }
      ts->sym = str;
      return ts->token = ltk_identify;
    default:
      continue;
    }
  }
}

int ltoken_next(ltoken_state* ts,vgc_heap* heap){
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
    return ltoken_lex_string(ts,heap);
  case '+':
  case '-':
    c = ltoken_look_ahead(ts);
    if(c < '0' || c > '9'){
      return ltoken_lex_identify(ts);
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
    return ltoken_lex_number(ts);
  case LEOF:
    return ts->token = ltk_eof;
  default:
    return ltoken_lex_identify(ts);
  }
}

vslot lparser_atom_parse(ltoken_state* ts,vgc_heap* heap){
  vslot s_exp = VSLOT_NULL;
  int   tk    = ts->token;
  switch(tk){
  case ltk_string:
    vslot_ref_set(s_exp,ts->str);
    return s_exp;
  case ltk_identify:
    {
      lsymbol* sym = lsymbol_new(heap,ts->sym);
      if(!sym){
	uabort("lparser_parse error!");
      }
      vslot_ref_set(s_exp,sym);
      return s_exp;
    }
  case ltk_number:
    vslot_num_set(s_exp,ts->num);
    return s_exp;
  default:
    ltoken_log(ts);
    uabort("lparser_parse error!");
  }
}

vslot lparser_exp_parse(ltoken_state* ts,vgc_heap* heap){
  vslot s_exp = VSLOT_NULL;
  lcons* cons = NULL;
  while(1){
    vslot car;
    int   tk = ltoken_next(ts,heap);
    switch(tk){
    case ltk_left:
      car = lparser_exp_parse(ts,heap);
      break;
    case ltk_right:
      return s_exp;
    default:
      car = lparser_atom_parse(ts,heap);
    }
    {
      lcons* tmp = lcons_new(heap);
      if(!tmp){
	uabort("lparser_parse error!");
      }
      if(!cons){
	vslot_ref_set(s_exp,tmp);
      }else{
	vslot_ref_set(cons->cdr,tmp);
      }
      cons = tmp;
      cons->car = car;
    }
  }
}

/*
 * s-exp := ( exp ) | atom
 * exp   := exp exp | s-exp | atom
*/
vslot lparser_parse(ltoken_state* ts,vgc_heap* heap){
  vslot s_exp = VSLOT_NULL;
  while(1){
    int tk = ltoken_next(ts,heap);
    switch(tk){
    case ltk_left:
      s_exp = lparser_exp_parse(ts,heap);
      return s_exp;
    default:
      s_exp = lparser_atom_parse(ts,heap);
      return s_exp;
    }
  }
}

void ltoken_log(ltoken_state* ts){
  char* sym;
  char* str;
  if(ts->sym){
    sym = ts->sym->value;
  }else{
    sym = "";
  }
  if(ts->str){
    str = ts->str->u.c;
  }else{
    str = "";
  }
  ulog ("********token state begin");
  ulog1("buffer :%s",ts->buf);
  ulog1("postion:%s",ts->pos);
  ulog1("index  :%d",(int)(ts->pos-ts->buf));
  ulog1("token  :%d",ts->token);
  ulog1("symbol :%s",sym);
  ulog1("string :%s",str);
  ulog1("number :%f",ts->num);
  ulog ("********token state   end");
}

void lparser_atom_log(vslot s_exp){
  if(vslot_is_ref(s_exp)){
    vgc_obj* obj = vslot_ref_get(s_exp);
    if(VGCTYPEOF(obj,gc_extend)){
      vgc_objex* objex = (vgc_objex*)obj;
      if(VGCTYPEOFEX(objex,lsymbol_type)){
	lsymbol* sym = (lsymbol*)objex;
	ulog1(" %s",sym->key->value);
      }else{
	ulog(" [objex]");
      }
    }else if(VGCTYPEOF(obj,gc_str)){
      vgc_str* str = (vgc_str*)obj;
      ulog1(" \"%s\"",str->u.c);
    }else{
      ulog(" [obj]");
    }
  }else if(vslot_is_null(s_exp)){
    ulog(" ()");
  }else if(vslot_is_num(s_exp)){
    double num = vslot_num_get(s_exp);
    ulog1(" %f",num);
  }else if(vslot_is_bool(s_exp)){
    int bool = vslot_bool_get(s_exp);
    ulog1(" %d",bool);
  }else{
    ulog("lparser log error!");
  }
}

void lparser_exp_log(vslot s_exp){
  lcons* cons;
  if(vslot_is_ref(s_exp)){
    vgc_obj* obj = vslot_ref_get(s_exp);
    if(VGCTYPEOF(obj,gc_extend)){
      vgc_objex* objex = (vgc_objex*)obj;
      if(VGCTYPEOFEX(objex,lcons_type)){
	cons = (lcons*)objex;
	ulog("(");
	while(1){
	  vslot slot;
	  lparser_exp_log(cons->car);
	  slot = cons->cdr;
	  if(vslot_is_ref(slot)){
	    vgc_obj* obj = vslot_ref_get(slot);
	    if(VGCTYPEOF(obj,gc_extend)){
	      vgc_objex* objex = (vgc_objex*)obj;
	      if(VGCTYPEOFEX(objex,lcons_type)){
		cons = (lcons*)objex;
		continue;
	      }
	    }
	  }else if(vslot_is_null(slot)){
	    ulog(")");
	    break;
	  }
	  lparser_atom_log(slot);
	  break;
	}
	return;
      }
    }
  }
  lparser_atom_log(s_exp);
}
