#include <cpu/exec.h>
#include <memory/vaddr.h>
#include <memory/paddr.h>
#include "../local-include/decode.h"
#include "all-instr.h"

static inline void set_width(DecodeExecState *s, int width) {
  if (width == -1) return;
  if (width == 0) {
    width = s->isa.is_operand_size_16 ? 2 : 4;
  }
  s->src1.width = s->dest.width = s->src2.width = width;
}

/* 0x80, 0x81, 0x83 */
static inline def_EHelper(gp1) {
  switch (s->isa.ext_opcode) { 
   
    
	  case 0:exec_add(s);break;
	  case 1:exec_or(s);break;
	  case 2:exec_adc(s);break;
	  case 3:exec_sbb(s);break;
	  case 4:exec_and(s);break;  //why:to make id_src1->width=1,to extend the id_src1
	  case 5:exec_sub(s);break;
	  case 6:exec_xor(s);break;
	  case 7:exec_cmp(s);break;

	  }

}

/* 0xc0, 0xc1, 0xd0, 0xd1, 0xd2, 0xd3 */
static inline def_EHelper(gp2) {

  switch (s->isa.ext_opcode) {
    
	  case 0:exec_rol(s);break;
	  case 1:exec_ror(s);break;
	  case 5:exec_shr(s);break;
	  case 7:exec_sar(s);break;
	  case 4:exec_shl(s);break; 
	  default :printf("case:%d\n",s->isa.ext_opcode);assert(0);
  }
}

/* 0xf6, 0xf7 */
static inline def_EHelper(gp3) {
  switch (s->isa.ext_opcode) {
     EMPTY(1)
	  
	  case 0:decode_test_I(s);exec_test(s);break;
	  case 2:exec_not(s);break;
	  case 3:exec_neg(s);break;
	  case 4:exec_mul(s);break;
	  case 5:exec_imul1(s);break;
	  case 6:exec_div(s);break;
	  case 7:exec_idiv(s);break;
	  default:assert(0);
  }
}

/* 0xfe */
static inline def_EHelper(gp4) {
  switch (s->isa.ext_opcode) {
    EMPTY(0)  EMPTY(2) EMPTY(3)
    EMPTY(4) EMPTY(5) EMPTY(6) EMPTY(7)
	  case 1:exec_dec(s);break;
      default:assert(0);
  }
}

/* 0xff */
static inline def_EHelper(gp5) 
{ 
  switch (s->isa.ext_opcode) {
   
    EMPTY(5) EMPTY(7)
	EX(0,inc)
    EX(1,dec)
	EX(2,call_rm)
	EX(3,call)
	EX(4,jmp_rm)
	EX(6,push)
    default:assert(0);
  }


}

/* 0x0f 0x01*/
static inline def_EHelper(gp7) {
  switch (s->isa.ext_opcode) {
    EMPTY(0) EMPTY(1)    EMPTY(4) EMPTY(5) EMPTY(7)
	case 2:exec_lgdt(s);break;
	case 3:exec_lidt(s);break;
    default:assert(0);
  }
}

static inline def_EHelper(2byte_esc) {
  uint8_t opcode = instr_fetch(&s->seq_pc, 1);
  s->opcode = opcode;

  switch (opcode) {
    IDEX(0x00,E,ltr)	
    IDEX (0x01, gp7_E, gp7)

    IDEX(0x20,mov_G2E,mov_cr2r)
	IDEX(0x22,mov_E2G,mov_r2cr)

	IDEXW(0x90,setcc_E,setcc,1) IDEXW(0x91,setcc_E,setcc,1) IDEXW(0x92,setcc_E,setcc,1) IDEXW(0x93,setcc_E,setcc,1)
    IDEXW(0x94,setcc_E,setcc,1) IDEXW(0x95,setcc_E,setcc,1) IDEXW(0x96,setcc_E,setcc,1) IDEXW(0x97,setcc_E,setcc,1)
    IDEXW(0x98,setcc_E,setcc,1) IDEXW(0x99,setcc_E,setcc,1) IDEXW(0x9a,setcc_E,setcc,1) IDEXW(0x9b,setcc_E,setcc,1)
    IDEXW(0x9c,setcc_E,setcc,1) IDEXW(0x9d,setcc_E,setcc,1) IDEXW(0x9e,setcc_E,setcc,1) IDEXW(0x9f,setcc_E,setcc,1)

	IDEX(0xb6,mov_byteE2G,movzx)
    
	IDEX(0xa4,Ib_G2E,shld) IDEX(0xa5,cl_G2E,shld)

	IDEX(0xaf,E2G,imul2)
	IDEX(0xbe,movsx,movsx)
	IDEX(0xbf,movswl,movsx)

	IDEX(0xb7,movzwl,movzx)

    IDEX(0x80,jcc_l,jcc) IDEX(0x81,jcc_l,jcc) IDEX(0x82,jcc_l,jcc) IDEX(0x83,jcc_l,jcc)
	IDEX(0x84,jcc_l,jcc) IDEX(0x85,jcc_l,jcc) IDEX(0x86,jcc_l,jcc) IDEX(0x87,jcc_l,jcc)
    IDEX(0x88,jcc_l,jcc) IDEX(0x89,jcc_l,jcc) IDEX(0x8a,jcc_l,jcc) IDEX(0x8b,jcc_l,jcc) 
    IDEX(0x8c,jcc_l,jcc) IDEX(0x8d,jcc_l,jcc) IDEX(0x8e,jcc_l,jcc) IDEX(0x8f,jcc_l,jcc)
    default: exec_inv(s);
  }
}

static inline void fetch_decode_exec(DecodeExecState *s) {
  uint8_t opcode;
again:
  opcode = instr_fetch(&s->seq_pc, 1);
  s->opcode = opcode;
 // if(opcode==0x0) printf("cpu.pc:%x\n",cpu.pc);
// if((cpu.pc>=0x3001000)&&(cpu.pc<=0x300139c)) printf("opcode:%x edi:%x eax:%x ebp:%x esp:%x\n\n",opcode,cpu.edi,cpu.eax,cpu.ebp,cpu.esp);
//   if(cpu.pc==0x101366) printf("opcode:%x\n",opcode);

  switch (opcode) {
    EX   (0x0f, 2byte_esc)//eacape code
		
    IDEXW(0x80, I2E, gp1, 1)     //sub,and... immidiate byte to r/m byte
    IDEX (0x81, I2E, gp1)        //sub,and..  immidiate word/dword to r/m word/dword
    IDEX (0x83, SI2E, gp1)       //sub,and... signed-extended immidiate byte to r/m word/dword

    IDEXW(0x88, mov_G2E, mov, 1)
    IDEX (0x89, mov_G2E, mov)
    IDEXW(0x8a, mov_E2G, mov, 1)
    IDEX (0x8b, mov_E2G, mov)
    IDEXW(0xa0, O2a, mov, 1)
    IDEX (0xa1, O2a, mov)
    IDEXW(0xa2, a2O, mov, 1)
    IDEX (0xa3, a2O, mov)
    IDEXW(0xc6, mov_I2E, mov, 1)
    IDEX (0xc7, mov_I2E, mov)
    IDEXW(0xb0, mov_I2r, mov, 1)
    IDEXW(0xb1, mov_I2r, mov, 1)
    IDEXW(0xb2, mov_I2r, mov, 1)
    IDEXW(0xb3, mov_I2r, mov, 1)
    IDEXW(0xb4, mov_I2r, mov, 1)
    IDEXW(0xb5, mov_I2r, mov, 1)
    IDEXW(0xb6, mov_I2r, mov, 1)
    IDEXW(0xb7, mov_I2r, mov, 1)
    IDEX (0xb8, mov_I2r, mov)
    IDEX (0xb9, mov_I2r, mov)
    IDEX (0xba, mov_I2r, mov)
    IDEX (0xbb, mov_I2r, mov)
    IDEX (0xbc, mov_I2r, mov)
    IDEX (0xbd, mov_I2r, mov)
    IDEX (0xbe, mov_I2r, mov)
    IDEX (0xbf, mov_I2r, mov)


    IDEXW(0xc0, gp2_Ib2E, gp2, 1)
    IDEX (0xc1, gp2_Ib2E, gp2)
    IDEXW(0xd0, gp2_1_E, gp2, 1)
    IDEX (0xd1, gp2_1_E, gp2)
    IDEXW(0xd2, gp2_cl2E, gp2, 1)
    IDEX (0xd3, gp2_cl2E, gp2)


    IDEXW(0xf6, E, gp3, 1)
    IDEX (0xf7, E, gp3)
    IDEXW(0xfe, E, gp4, 1)

    IDEX (0xff, E, gp5)

    EX   (0xd6, nemu_trap)
    //control
	IDEXW(0x70,jcc,jcc,1) IDEXW(0x71,jcc,jcc,1) IDEXW(0x72,jcc,jcc,1) IDEXW(0x73,jcc,jcc,1) IDEXW(0x74,jcc,jcc,1) IDEXW(0x75,jcc,jcc,1) IDEXW(0x76,jcc,jcc,1) IDEXW(0x77,jcc,jcc,1)
    IDEXW(0x78,jcc,jcc,1) IDEXW(0x79,jcc,jcc,1) IDEXW(0x7a,jcc,jcc,1) IDEXW(0x7b,jcc,jcc,1) IDEXW(0x7c,jcc,jcc,1) IDEXW(0x7d,jcc,jcc,1) IDEXW(0x7e,jcc,jcc,1) IDEXW(0x7f,jcc,jcc,1)

	IDEX(0xe8,jcc,call) IDEX(0xe9,jcc,jmp) IDEXW(0xeb,jcc,jmp,1)

	EX(0xc3,ret)

    //data-mov

    IDEX(0x50,push_r,push) IDEX(0x51,push_r,push) IDEX(0x52,push_r,push) IDEX(0x53,push_r,push) IDEX(0x54,push_r,push) IDEX(0x55,push_r,push) IDEX(0x56,push_r,push) IDEX(0x57,push_r,push)
    IDEX(0x58,pop_r,pop) IDEX(0x59,pop_r,pop) IDEX(0x5a,pop_r,pop) IDEX(0x5b,pop_r,pop) IDEX(0x5c,pop_r,pop) IDEX(0x5d,pop_r,pop) IDEX(0x5e,pop_r,pop) IDEX(0x5f,pop_r,pop)
	IDEX(0x68,push_I,push) IDEXW(0x6a,push_I,push,1)
    EX(0x60,pusha) EX(0x61,popa)
    EX(0x98,cwtl)  EX(0x99,cltd)
	IDEX(0x8d,lea,lea)
	EX(0xc9,leave)

	IDEX(0x90,r,xchg) IDEX(0x94,r,xchg)
	EXW(0xa4,movsb,1) EX(0xa5,movsb)
    //logic-r-rm
	IDEXW(0x08,G2E,or,1) IDEX(0x09,G2E,or)  IDEXW(0x0a,E2G,or,1) IDEX(0x0b,E2G,or) IDEXW(0x0c,I2a,or,1) IDEX(0x0d,I2a,or)
    IDEXW(0x20,G2E,and,1) IDEX(0x21,G2E,and) IDEXW(0x22,E2G,and,1) IDEX(0x23,E2G,and) IDEX(0x25,I2a,and)
	IDEXW(0x30,xor_r2rm,xor,1) IDEX(0x31,xor_r2rm,xor) IDEXW(0x32,xor_rm2r,xor,1) IDEX(0x33,xor_rm2r,xor)  IDEX(0x35,I2a,xor)
    IDEX(0x85,G2E,test)	IDEXW(0x84,G2E,test,1) IDEXW(0xa8,I2a,test,1) IDEX(0xa9,I2a,test)
	
	
    //arith-r-rm
	IDEXW(0x38,cmp_r2rm,cmp,1) IDEX(0x39,cmp_r2rm,cmp) IDEXW(0x3a,cmp_rm2r,cmp,1) IDEX(0x3b,cmp_rm2r,cmp ) IDEXW(0x3c,cmp_I2a,cmp,1) IDEX(0x3d,cmp_I2a,cmp) 
	IDEX(0x40,inc_r,inc) IDEX(0x41,inc_r,inc) IDEX(0x42,inc_r,inc) IDEX(0x43,inc_r,inc) IDEX(0x44,inc_r,inc) IDEX(0x45,inc_r,inc) IDEX(0x46,inc_r,inc) IDEX(0x47,inc_r,inc)
    IDEX(0x48,dec_r,dec) IDEX(0x49,dec_r,dec) IDEX(0x4a,dec_r,dec) IDEX(0x4b,dec_r,dec) IDEX(0x4c,dec_r,dec) IDEX(0x4d,dec_r,dec) IDEX(0x4e,dec_r,dec) IDEX(0x4f,dec_r,dec)

    IDEXW(0x10,G2E,adc,1)	IDEX(0x11,G2E,adc)  IDEXW(0x12,E2G,adc,1) IDEX(0x13,E2G,adc)
	IDEXW(0x00,add_r2rm,add,1) IDEX(0x01,add_r2rm,add) IDEXW(0x02,add_rm2r,add,1) IDEX(0x03,add_rm2r,add) IDEXW(0x04,I2a,add,1) IDEX(0x05,I2a,add)

    IDEX(0x19,G2E,sbb)  IDEX(0x1b,E2G,sbb)
	IDEX(0x29,G2E,sub) IDEX(0x2b,E2G,sub) IDEXW(0x2c,I2a,sub,1) IDEX(0x2d,I2a,sub)

	IDEX(0x69,I_E2G,imul3)

	//system
 	IDEXW(0xcd,I,int,1);
    EX(0xcf,iret);

    //IOE
	IDEXW(0xe4,in_I2a,in,1) IDEX(0xe5,in_I2a,in) IDEXW(0xe6,out_a2I,out,1) IDEX(0xe7,out_a2I,out)
	IDEXW(0xec,in_dx2a,in,1) IDEX(0xed,in_dx2a,in) IDEXW(0xee,out_a2dx,out,1) IDEX(0xef,out_a2dx,out)

  case 0x66: s->isa.is_operand_size_16 = true; goto again;
  default: exec_inv(s);
  }
}

extern void query_intr(DecodeExecState *s);

  vaddr_t isa_exec_once() {

  DecodeExecState s;
  s.is_jmp = 0;
  s.isa = (ISADecodeInfo) { 0 };
  s.seq_pc = cpu.pc;             //start at cpu.pc

  fetch_decode_exec(&s);
  update_pc(&s); //cpu.pc=s.seq_pc or jmp_pc

  query_intr(&s);

  return s.seq_pc;
}





