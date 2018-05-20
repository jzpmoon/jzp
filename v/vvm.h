#ifndef _VVM_H_
#define _VVM_H_

enum ir_type{
  opcode,reg,imm,mem,
};

enum op_or_reg_type{
  reg_stack,
  reg_pc,
  reg_arithmetic,
  op_transfer,
  op_arithmetic,
};

typedef struct _memory{
  int len:
  ustring* n;
  ustring* v;
} memory;

typedef struct _vir{
  enum ir_type it;
  enum op_or_reg_type ot;
  memory* m;
  ustring* n;
  int len;
  vir* i[1];
} vir;

typedef struct _block{
  vir* opcode;
  vir* operand;
} block;

typedef struct _module{
  memory* mem;
  block* bck;
  block* entry;
} module;

#endif
