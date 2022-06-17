#include "cc.h"

static inline def_EHelper(jmp) {
  // the target address is calculated at the decode stage
  rtl_j(s, s->jmp_pc);

  print_asm("jmp %x", s->jmp_pc);
}

static inline def_EHelper(jcc) {
  // the target address is calculated at the decode stage
  uint32_t cc = s->opcode & 0xf;
  rtl_setcc(s, s0, cc);
  //depend cc(subcode),we konw the condition and next we know if the contition is met
  //if is met,*s0=1,else *s0=0;

  rtl_jrelop(s, RELOP_NE, s0, rz, s->jmp_pc);

}

static inline def_EHelper(jmp_rm) {
  if(cpu.pc==0x100173){
  int  val=vaddr_read(0x3000000,4);
  printf("jmp_rm,dest:%x val:%x\n",id_dest->val,val);
  }
  rtl_jr(s, &id_dest->val);

  print_asm("jmp *%s", id_dest->str);
}

static inline def_EHelper(call) {
  // the target address is calculated at the decode stage
  //  rtl_jr(s,dsrc1);
  /*
  rtl_push(s,&cpu.pc);
  rtl_j(s,id_src1->val);
  */
  rtl_push(s,&s->seq_pc);
  rtl_j(s,s->jmp_pc);
  print_asm("call %x", s->jmp_pc);

}

static inline def_EHelper(ret) {
   rtl_pop(s,&id_dest->val);
  // printf("id_dest->val:%x\n",id_dest->val);
   rtl_j(s,id_dest->val);
  print_asm("ret");
}

static inline def_EHelper(ret_imm) {
  TODO();
  print_asm("ret %s", id_dest->str);
}

static inline def_EHelper(call_rm) {
  s->is_jmp=1;
  s->jmp_pc=id_dest->val;
  if(cpu.pc==0x10050d) printf("call_rm,dest:%x\n",id_dest->val);
//  printf("call_rm,id_dest->val:%x\n",s->jmp_pc);
//  printf("call_rm,s->jmp_pc:%x\n",s->jmp_pc);
  rtl_push(s,&s->seq_pc);

  print_asm("call *%s", id_dest->str);
}
