#ifndef __CPU_DECODE_H__
#define __CPU_DECODE_H__

#include <isa.h>

#define OP_STR_SIZE 40
enum { OP_TYPE_REG, OP_TYPE_MEM, OP_TYPE_IMM };//	register,memory,immediate

typedef struct {
  uint32_t type;
  int width;
  union {
    uint32_t reg;
    word_t imm;
    sword_t simm;
  };

  rtlreg_t *preg;             //reg pointer
  rtlreg_t val;               //preg point to val if preg cant point to reg directly

  char str[OP_STR_SIZE];
} Operand;


typedef struct {
  uint32_t opcode;

  vaddr_t seq_pc;  // sequential pc,modified in instr_fetch().
  uint32_t is_jmp;
  vaddr_t jmp_pc;

  Operand src1, dest, src2; //Operand,why 3?
  int width;

  rtlreg_t tmp_reg[7]; //temporary reg

  ISADecodeInfo isa; //x86_ISADecodeInfo
} DecodeExecState;

/*include/isa/x86.h
 
typedef struct {
  bool is_operand_size_16;
  uint8_t ext_opcode;
  const rtlreg_t *mbase;
  rtlreg_t mbr;
  word_t moff;
} x86_ISADecodeInfo;
*/



#define def_DHelper(name) void concat(decode_, name) (DecodeExecState *s)   //decode helper





#ifdef DEBUG
#define print_Dop(...) snprintf(__VA_ARGS__)
#else
#define print_Dop(...)
#endif

#endif
