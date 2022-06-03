#ifndef _USET_H_
#define _USET_H_

#include "umacro.h"

typedef struct _uset uset;
typedef struct _ucursor ucursor;

typedef void (ucall* uset_cursor_ft)(ucursor* cursor);
typedef void* (ucall* uset_next_ft)(uset* set,ucursor* cursor);

struct _ucursor{
  char pad[32];  
};

#define USETHEADER				\
  uset_cursor_ft iterate;			\
  uset_next_ft next

struct _uset{
  USETHEADER;
};

typedef void** unext;
#define unext_get(next) *(next);

#define UMAPHEADER(t)							\
  USETHEADER;								\
  umap_##t##_put_ft put;						\
  umap_##t##_get_ft get

#define umap_tpl(t)							\
  typedef struct _umap_##t umap_##t;					\
  typedef t (ucall* umap_##t##_key_ft)(t*);				\
  typedef int (ucall* umap_##t##_comp_ft)(t*,t*);			\
  typedef int (ucall* umap_##t##_put_ft)(umap_##t* map,			\
					 unsigned int hscd,		\
					 t* in,				\
					 t** out,			\
					 umap_##t##_key_ft put,		\
					 umap_##t##_comp_ft comp);	\
  typedef int (ucall* umap_##t##_get_ft)(umap_##t* map,			\
					 unsigned int hscd,		\
					 t* in,				\
					 t** out,			\
					 umap_##t##_comp_ft comp);	\
  struct _umap_##t{							\
    UMAPHEADER(t);							\
  }

#define umap_decl_tpl(t)			\
  umap_tpl(t)

#endif
