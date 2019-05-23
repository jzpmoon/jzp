#ifndef _VTOKEN_H_
#define _VTOKEN_H_

enum vtoken{
	    tk_indentify,
	    tk_left_kg,
	    tk_right_kg,
	    tk_string,
	    tk_number,
};

typedef struct _vtoken_state{
  char* buf;
  char* pos;
  int token;
} vtoken_state;

#endif
