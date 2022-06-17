#include <cpu/exec.h>
#include <monitor/monitor.h>
#include <monitor/difftest.h>

def_EHelper(nop) {
  print_asm("nop");
}

def_EHelper(inv) {
  /* invalid opcode */

  uint32_t temp[2];
  s->seq_pc = cpu.pc;
  temp[0] = instr_fetch(&s->seq_pc, 4);
  temp[1] = instr_fetch(&s->seq_pc, 4);

  uint8_t *p = (void *)temp;
  printf("invalid opcode(PC = 0x%08x): %02x %02x %02x %02x %02x %02x %02x %02x ...\n\n",
      cpu.pc, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);

  display_inv_msg(cpu.pc);

  rtl_exit(NEMU_ABORT, cpu.pc, -1);
  
  /*
   rtl_exit(int state,vaddr_t halt_pc, uint32_t halt_ret){
     nemu_state=(NEMU){.state=state,.halt_pc=halt_pc,.halt_ret=halt_ret};
   }
  */

  print_asm("invalid opcode");
}

def_EHelper(nemu_trap) {
  difftest_skip_ref();

  rtl_exit(NEMU_END, cpu.pc, cpu.eax);

  print_asm("nemu trap");
  return;
}
