#include <cpu/exec.h>
#include "local-include/rtl.h"
#include "local-include/reg.h"
#include "local-include/decode.h"

typedef union {
  struct {
    uint8_t R_M		:3;
    uint8_t reg		:3;
    uint8_t mod		:2;
  };
  struct {
    uint8_t dont_care	:3;
    uint8_t opcode		:3;
  };
  uint8_t val;
} ModR_M;

typedef union {
  struct {
    uint8_t base	:3;
    uint8_t index	:3;
    uint8_t ss		:2;
  };
  uint8_t val;
} SIB;

static inline void load_addr(DecodeExecState *s, ModR_M *m, Operand *rm) {
  assert(m->mod != 3);

  sword_t disp = 0;
  int disp_size = 4;
  int base_reg = -1, index_reg = -1, scale = 0;

  if (m->R_M == R_ESP) {      //if m->R_M=100, means need  SIB,get SIB by instr_fetch
    SIB sib;
    sib.val = instr_fetch(&s->seq_pc, 1);
    base_reg = sib.base;
    scale = sib.ss;

    if (sib.index != R_ESP) { index_reg = sib.index; }   //special case:see SIB chart,if index_reg=100,then no index_reg (but SIB exist,mean what?)
  }
  else {   //* no SIB ,only a base_reg(that is R_M) plus a disp is enough
    base_reg = m->R_M;
  }

  //special case :m->mod=0.   in this case:u can onlu choose one of base_reg and disp!!!
  if (m->mod == 0) {  //R_EBP=101 
    if (base_reg == R_EBP) { base_reg = -1; }
	//if no SIB(R_M!=100; R_M=101; base_reg=R_M=101), disp exist,but no base_reg;;;;even if get SIB(R_M=100;base_reg=sib.base=101),no disp,no base_reg too
    else { disp_size = 0; }                   //no disp
  }
  else if (m->mod == 1) { disp_size = 1; }



  if (disp_size != 0) {
    /* has disp */
    disp = instr_fetch(&s->seq_pc, disp_size);
    if (disp_size == 1) { disp = (int8_t)disp; }
  }



  s->isa.mbase = (base_reg != -1 ? &reg_l(base_reg) : rz);
 
 // if(cpu.pc==0x10019c) printf("come in load_addr!");

  if (index_reg != -1) {                             //guese get a (index_reg*Scale +base_reg)
    rtl_shli(s, s1, &reg_l(index_reg), scale); //s1=scale*reg_l(index_reg)   note: s1 =&s->tmp_reg[1]
    rtl_add(s, &s->isa.mbr, s->isa.mbase, s1); //&s->isa.mbr=s->isa.mbase+s1
    s->isa.mbase = &s->isa.mbr;
  }
  s->isa.moff = disp;

#ifdef DEBUG
  char disp_buf[16];
  char base_buf[8];
  char index_buf[8];

  if (disp_size != 0) {
    /* has disp */
    sprintf(disp_buf, "%s%#x", (disp < 0 ? "-" : ""), (disp < 0 ? -disp : disp));
  }
  else { disp_buf[0] = '\0'; }

  if (base_reg == -1) { base_buf[0] = '\0'; }
  else { 
    sprintf(base_buf, "%%%s", reg_name(base_reg, 4));
  }

  if (index_reg == -1) { index_buf[0] = '\0'; }
  else { 
    sprintf(index_buf, ",%%%s,%d", reg_name(index_reg, 4), 1 << scale);
  }

  if (base_reg == -1 && index_reg == -1) {
    sprintf(rm->str, "%s", disp_buf);
  }
  else {
    sprintf(rm->str, "%s(%s%s)", disp_buf, base_buf, index_buf);
  }
#endif

  rm->type = OP_TYPE_MEM;
}

void read_ModR_M(DecodeExecState *s, Operand *rm, bool load_rm_val, Operand *reg, bool load_reg_val) {
  ModR_M m;
  m.val = instr_fetch(&s->seq_pc, 1);
  s->isa.ext_opcode = m.opcode;       //extension code

  if (reg != NULL) operand_reg(s, reg, load_reg_val, m.reg, reg->width); 
  //opcode/reg means reg   (reg!=NULL means need a reg as op,reg deliver by paremeter(Operand *reg) because it is opcode decide opcode/reg is reg or not,not ModRM)
  //at this time,m.reg is a reg(int)

  if (m.mod == 3){ operand_reg(s, rm, load_rm_val, m.R_M, rm->width); }     
  //rm means r  (m.mod==3 means need a reg as op,rm not deliver by paremeter because it is ModRM(mod) decide opcode/reg is reg or not)
  //at this time,m.R_Mis a reg(int)
  
  else {   //rm means m (m.mod!=3 means need a mem as op)
    load_addr(s, &m, rm);

    if (load_rm_val) rtl_lm(s, &rm->val, s->isa.mbase, s->isa.moff, rm->width); 	//the meaning of load_addr is get address(s->isa.mbase,s->isa.moff),then go to pmem get the value to put in val!!!
  
    rm->preg = &rm->val;
  }
}
