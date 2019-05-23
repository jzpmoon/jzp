#ifndef VBYTE_CODE

#define VBYTE_CODE							\
  DF(Bnop,"nop",0x00,1)							\
  DF(Bload,"load",0x01,2)						\
  DF(Bstore,"store",0x02,2)						\
  DF(Bpush,"push",0x03,2)						\
  DF(Bpop,"pop",0x04,1)							\
  DF(Bjmp,"jmp",0x05,3)					                \
  DF(Bjmpi,"jmpi",0x06,3)				      		\
  DF(Beq,"eq",0x07,1)							\
  DF(Bgt,"gt",0x08,1)							\
  DF(Blt,"lt",0x09,1)							\
  DF(Band,"and",0x0B,1)							\
  DF(Bor,"or",0x0C,1)							\
  DF(Bnot,"not",0x0D,1)							\
  DF(Badd,"add",0x0E,1)							\
  DF(Bsub,"sub",0x0F,1)							\
  DF(Bmul,"mul",0x10,1)						        \
  DF(Bdiv,"div",0x11,1)							\
  DF(Bcall,"call",0x12,1)						\
  DF(Breturn,"return",0x13,1)						\
  DF(Bretvoid,"retvoid",0x14,1)						\
  DF(Bref,"ref",0x15,3)						        \
  DF(Bset,"set",0x16,3)
#endif
