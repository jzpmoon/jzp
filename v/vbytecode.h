#ifndef VBYTECODE

#define VBYTECODE							\
  DF(Bload,"load",0004,2,1)						\
  DF(Bstore,"store",0010,2,1)						\
  DF(Bpush,"push",0014,2,1)						\
  DF(Bpopv,"popv",0020,1,0)						\
  DF(Bpop,"pop",0024,2,1)							\
  DF(Bjmp,"jmp",0030,3,1)					                \
  DF(Bjmpi,"jmpi",0034,3,1)				      		\
  DF(Brefl,"refl",0037,3,1)						\
  DF(Bref,"ref",0040,3,1)						        \
  DF(Bsetl,"setl",0043,3,1)						\
  DF(Bset,"set",0044,3,1)							\
  DF(Btop,"top",0050,2,1)							\
    									\
  DF(Beq,"eq",0362,1,0)							\
  DF(Bgt,"gt",0363,1,0)							\
  DF(Blt,"lt",0364,1,0)							\
  DF(Band,"and",0365,1,0)							\
  DF(Bor,"or",0366,1,0)							\
  DF(Bnot,"not",0367,1,0)							\
  DF(Badd,"add",0370,1,0)							\
  DF(Bsub,"sub",0371,1,0)							\
  DF(Bmul,"mul",0372,1,0)						        \
  DF(Bdiv,"div",0373,1,0)							\
  DF(Bcall,"call",0374,1,0)						\
  DF(Breturn,"return",0375,1,0)						\
  DF(Bretvoid,"retvoid",0376,1,0)						\
  DF(Bnop,"nop",0377,1,0)							\
  									\
  DF(Vlabel,"label",0400,2,1)

#define vbytecode_is_vaild(value)		\
  (value < 0400)

#endif
