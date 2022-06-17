#include <am.h>
#include <nemu.h>

extern char _heap_start;
int main(const char *args);

Area heap = RANGE(&_heap_start, PMEM_END); 

#ifndef MAINARGS
#define MAINARGS ""
#endif
static const char mainargs[] = MAINARGS;

void putch(char ch) {
  outb(SERIAL_PORT, ch); //SERIAL_PORT is defined in nemu.h
}

void halt(int code) {
  nemu_trap(code);     //nemu_trap()is a macro in am/src/nemu/include/nemu.h    (halt is am level,nemu_trap(code) is ISA level)

  //nemu_trap(code)  to  0xd6 a(code)  to  exec_nemu_trap() to rtl_exit(NEMU_END,cpu.pc,cpu.eax);;;;;; (a(code) means code to eax in x86)
  //
  /*1:# define nemu_trap(code) asm volatile (".byte 0xd6" : :"a"(code));0xd6 means exec_nemu_trap(s).
   *2:def_EHelper(nemu_trap){
      difftest_skip_ref();
	  rtl_exit(NEMU_END,cpu.pc,cpu.eax);
	  print_asm("nemu_trap");
	  return;
	 }
   *3:void rtl_exit(int state,vaddr_t halt_pc,uint32_t halt_ret){
      nemu_state=(NEMU){.state=state, .halt_pc=halt_pc, .halt_ret=halt_ret};
	  }

   *4:get out of cpu_exec();

   */

  // should not reach here
  while (1);
}

void _trm_init() {
  int ret = main(mainargs);
  halt(ret);
}
