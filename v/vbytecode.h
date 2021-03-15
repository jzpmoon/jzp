#ifndef VBYTECODE

#define VBYTECODE							\
  DF(Bload,"load",0004,2)						\
  DF(Bstore,"store",0010,2)						\
  DF(Bpush,"push",0014,2)						\
  DF(Bpopv,"popv",0020,1)						\
  DF(Bpop,"pop",0024,2)							\
  DF(Bjmp,"jmp",0030,3)					                \
  DF(Bjmpi,"jmpi",0034,3)				      		\
  DF(Brefl,"refl",0037,3)						\
  DF(Bref,"ref",0040,3)						        \
  DF(Bsetl,"setl",0043,3)						\
  DF(Bset,"set",0044,3)							\
  DF(Btop,"top",0050,2)							\
    									\
  DF(Beq,"eq",0362,1)							\
  DF(Bgt,"gt",0363,1)							\
  DF(Blt,"lt",0364,1)							\
  DF(Band,"and",0365,1)							\
  DF(Bor,"or",0366,1)							\
  DF(Bnot,"not",0367,1)							\
  DF(Badd,"add",0370,1)							\
  DF(Bsub,"sub",0371,1)							\
  DF(Bmul,"mul",0372,1)						        \
  DF(Bdiv,"div",0373,1)							\
  DF(Bcall,"call",0374,1)						\
  DF(Breturn,"return",0375,1)						\
  DF(Bretvoid,"retvoid",0376,1)						\
  DF(Bnop,"nop",0377,1)
#endif
