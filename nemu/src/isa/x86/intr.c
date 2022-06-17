#include <cpu/exec.h>
#include "local-include/rtl.h"
//#include "exec/system.h"

#define IRQ_TIMER 32


void raise_intr(DecodeExecState *s, word_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  /*
  rtl_push(s,&cpu.EFLAGS);
  cpu.IF=0;
  rtl_push(s,(uint32_t*)&cpu.cs);
  rtl_push(s,&ret_addr);

  uint32_t a=vaddr_read(cpu.IDTR.base+8*NO,4);
  uint32_t b=vaddr_read(cpu.IDTR.base+8*NO+4,4);
  uint32_t entry_addr=(a&0xffff)|(b&0xffff0000);
  rtl_j(s,entry_addr);
  */
  vaddr_t gdt_addr=cpu.GDTR.addr+cpu.TR;  //what is TR reg?
  rtl_li(s,s1,gdt_addr);
  rtl_lm(s,s0,s1,0,4);
  vaddr_t Tss_addr=(((*s0)&0xffff0000)>>16);//get low bits of segment base address
  rtl_lm(s,s0,s1,4,4);
  Tss_addr+=(((*s0)&0x000000ff)<<16);//get middle bits of segment base address
  Tss_addr+=((*s0)&0xff000000);      //get high bits of segment base address
  rtl_li(s,s1,Tss_addr);//get Tss_addr

  rtl_lm(s,s0,s1,4,4);
  vaddr_t ksp=*s0;//get ksp,ksp=tss->esp0
 // printf("ksp value:%d\n",ksp);
  vaddr_t tep=cpu.esp;
  if(ksp!=0) cpu.esp=ksp; //switch to kernel stack

  rtl_lm(s,s0,s1,8,4);
  rtl_li(s,s1,*s0);
  rtl_push(s,s1);//push ss
  rtl_li(s,s0,tep);
  rtl_push(s,s0);//save  user stack 
  rtl_li(s,s0,cpu.EFLAGS);
  rtl_push(s,s0);//push EFLAGS
  cpu.IF=0;

  rtl_li(s,s0,(ksp==0)?8:3);
  rtl_push(s,s0);//push cs
  rtl_li(s,s0,(s->is_jmp?s->jmp_pc:s->seq_pc));
  rtl_push(s,s0);//push eip

  rtl_li(s,s1,ret_addr);
  rtl_lm(s,s0,s1,NO*8,4);
  vaddr_t Jpc=(*s0)&0xffff;
  rtl_lm(s,s0,s1,NO*8+4,4);
  Jpc+=(*s0)&(0xffff<<16);
  rtl_j(s,Jpc);//jmp to gate

  rtl_li(s,s0,0);
  rtl_li(s,s1,Tss_addr);
  rtl_sm(s,s1,4,s0,4);//support re-entry of CTE

}

void query_intr(DecodeExecState *s) {
     if(cpu.INTR && cpu.IF){ //what is the meaning of IF?   interrupt flag!
			 cpu.INTR=false;
			 raise_intr(s,IRQ_TIMER,cpu.IDTR.base);
			 update_pc(s);
	 }
}
