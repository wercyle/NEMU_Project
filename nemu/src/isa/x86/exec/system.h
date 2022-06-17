#include <monitor/difftest.h>

uint32_t pio_read_l(ioaddr_t);
uint32_t pio_read_w(ioaddr_t);
uint32_t pio_read_b(ioaddr_t);
void pio_write_l(ioaddr_t, uint32_t);
void pio_write_w(ioaddr_t, uint32_t);
void pio_write_b(ioaddr_t, uint32_t);



static inline def_EHelper(ltr){
		rtl_li(s,s1,0);
		rtl_add(s,s0,s1,ddest);
		cpu.TR=(uint16_t)*s0;

		print_asm_template1(ltr);
}


static inline def_EHelper(lidt) {
  uint16_t size=vaddr_read(*s->isa.mbase,2);
  uint32_t address=vaddr_read(*s->isa.mbase+2,4);
  if(s->isa.is_operand_size_16){
     cpu.IDTR.limit=size;
	 cpu.IDTR.base=address&0xffffff;
  }
  else{
     cpu.IDTR.limit=size;
	 cpu.IDTR.base=address;
  }
  print_asm_template1(lidt);
}

static inline def_EHelper(lgdt) {
	uint16_t size=vaddr_read(*s->isa.mbase,2);
	uint32_t address=vaddr_read(*s->isa.mbase+2,4);
	if(s->isa.is_operand_size_16){
		cpu.GDTR.size=size;
		cpu.GDTR.addr=address&0xffffff;
	}
	else{
		cpu.GDTR.size=size;
		cpu.GDTR.addr=address;
	}
	print_asm_template1(lgdt);
}



static inline def_EHelper(mov_r2cr) {     //0x22
  assert(id_dest->reg==3||id_dest->reg==0);
  assert(s->src1.width==4);
  rtl_lr(s,s1,id_src1->reg,4);

  if(id_dest->reg==3) cpu.cr3=(uint32_t)*s1;
  else {
		  cpu.cr0=(uint32_t)*s1;
}

  print_asm("movl %%%s,%%cr%d", reg_name(id_src1->reg, 4), id_dest->reg);
}

static inline def_EHelper(mov_cr2r) {       //0x20
  assert(id_src1->reg==0||id_src1->reg==3);
  assert(s->dest.width==4);

  if(id_src1->reg==0) rtl_li(s,s0,cpu.cr0);
  else rtl_li(s,s0,cpu.cr3);

  rtl_sr(s,id_dest->reg,s0,4);

  print_asm("movl %%cr%d,%%%s", id_src1->reg, reg_name(id_dest->reg, 4));

#ifndef __DIFF_REF_NEMU__
  difftest_skip_ref();
#endif
}




//void raise_intr(DecodeExecState *s, word_t NO, vaddr_t ret_addr);
static inline def_EHelper(int) {

  raise_intr(s,id_dest->val,cpu.IDTR.base);

  print_asm("int %s", id_dest->str);

#ifndef __DIFF_REF_NEMU__
  difftest_skip_dut(1, 2);
#endif
}

static inline def_EHelper(iret) {
/*
   rtl_pop(s,&s->jmp_pc);
   rtl_pop(s,(uint32_t*)&cpu.cs);
   s->is_jmp=1;
   rtl_pop(s,&cpu.EFLAGS);
*/
  rtl_pop(s,s0);
  s->jmp_pc=*s0; //pop eip
  s->is_jmp=true;
  rtl_pop(s,s0); //pop cs
  cpu.cs=*s0;
  rtl_pop(s,s0); //pop eflags
  cpu.EFLAGS=*s0;

  vaddr_t gdt_addr=cpu.GDTR.addr+cpu.TR;
  rtl_li(s,s1,gdt_addr);
  rtl_lm(s,s0,s1,0,4);
  vaddr_t Tss_addr=(((*s0)&0xffff0000)>>16);//get low bits of segment base address
  rtl_lm(s,s0,s1,4,4);
  Tss_addr+=(((*s0)&0x000000ff)<<16);//get middle bits of segment base address
  Tss_addr+=((*s0)&0xff000000);      //get high bits of segment base address
  rtl_li(s,s1,Tss_addr);  //get Tss_addr

  rtl_pop(s,s0); //pop esp 
  vaddr_t tep=*s0;
  rtl_pop(s,s0); //pop ss
  rtl_sm(s,s1,8,s0,4);

  rtl_li(s,s1,Tss_addr);
  rtl_li(s,s0,cpu.esp);
  if((cpu.cs&0x3)==0x3) rtl_sm(s,s1,4,s0,4); //save the kernel stack(ksp) 
  if(tep) cpu.esp=tep;  //switch to user stack

  print_asm("iret");

#ifndef __DIFF_REF_NEMU__
  difftest_skip_ref();
#endif
}

static inline def_EHelper(in) {
  
  if(id_dest->width==1)      *t1=pio_read_b(id_src1->val);
  else if(id_dest->width==2) *t1=pio_read_w(id_src1->val);
  else if(id_dest->width==4) *t1=pio_read_l(id_src1->val);

  operand_write(s,id_dest,t1);
 // if(*t1!=0)printf("in:%x\n",*t1);
  print_asm_template2(in);
}

static inline def_EHelper(out) {
  //pio_write_b(cpu.gpr[2]._16,id_src1->val);
  //printf("dest:%x src:%x\n",id_dest->val,id_src1->val);
  if(id_dest->width==1) pio_write_b(id_dest->val,id_src1->val);
  else if(id_dest->width==2)pio_write_w(id_dest->val,id_src1->val);
  else if(id_dest->width==4)pio_write_l(id_dest->val,id_src1->val);
 
  print_asm_template2(out);
}
