#include "cc.h"

static inline def_EHelper(test) {
  rtl_and(s,t1,&id_dest->val,&id_src1->val);

  cpu.OF=0;
  cpu.CF=0;
  rtl_update_ZFSF(s,t1,id_dest->width);
  print_asm_template2(test);
}

static inline def_EHelper(and) {
  if(id_src1->width==1){rtl_sext(s,&id_src1->val,&id_src1->val,1); }

  rtl_and(s,t1,&id_dest->val,&id_src1->val);
  operand_write(s,id_dest,t1);

  

  cpu.OF=0;
  cpu.CF=0;
  rtl_update_ZFSF(s,t1,id_dest->width);

  print_asm_template2(and);
}

static inline def_EHelper(xor) 
{   rtl_xor(s,t1,&id_dest->val,&id_src1->val);
	operand_write(s,id_dest,t1);

	cpu.OF=0;
	cpu.CF=0;
	rtl_update_ZFSF(s,t1,id_dest->width);
    print_asm_template2(xor);
}

static inline def_EHelper(or) {
  rtl_or(s,t1,&id_dest->val,&id_src1->val);
  operand_write(s,id_dest,t1);

    cpu.OF=0;
	cpu.CF=0;
	rtl_update_ZFSF(s,t1,id_dest->width);

  print_asm_template2(or);
}

static inline def_EHelper(not) {
  rtl_mv(s,t1,&id_dest->val);
  rtl_not(s,t1,t1);

  operand_write(s,id_dest,t1);
  print_asm_template1(not);
}

static inline def_EHelper(sar) {
  // unnecessary to update CF and OF in NEMU
  rtl_sar(s,t1,&id_dest->val,&id_src1->val);
  operand_write(s,id_dest,t1);

  rtl_update_ZFSF(s,t1,id_dest->width);
 // if(cpu.pc ==0x10024f) printf("sar,if_dest:%x\n",id_dest->val);

  print_asm_template2(sar);
}

static inline def_EHelper(shl) {
  // unnecessary to update CF and OF in NEMU
  rtl_shl(s,t1,&id_dest->val,&id_src1->val);
  operand_write(s,id_dest,t1);

  rtl_update_ZFSF(s,t1,id_dest->width);

  print_asm_template2(shl);
}

static inline def_EHelper(shr) {
  // unnecessary to update CF and OF in NEMU
  rtl_shr(s,t1,&id_dest->val,&id_src1->val);
  operand_write(s,id_dest,t1);

  rtl_update_ZFSF(s,t1,id_dest->width);
  print_asm_template2(shr);
}


static inline def_EHelper(shld){
	//if(cpu.pc==0x0012e24d) printf("1::dest:%x dest->val:%x src1:%x src2:%x cpu.ecx:%d\n",*ddest,id_dest->val,id_src1->val,id_src2->val,cpu.ecx);
	*t1=31;

	rtl_and(s,t1,t1,&id_src1->val);
	printf("t1:%d width:%d\n",*t1,id_dest->width);

	if(*t1==0){}
	else {
		if(*t1>=id_dest->width*8) assert(0);
		else{
			rtl_geti(s,t2,ddest,id_dest->width-*t1);
			int i=id_dest->width-1;
			while(i>=*t1){
               rtl_geti(s,t2,ddest,i-*t1);
               rtl_puti(s,ddest,t2,i);
			   i--;
			}
			while(i>=0){
               rtl_geti(s,t2,&id_src2->val,i-*t1+id_dest->width);
			   rtl_puti(s,ddest,t2,i);
			   i--;
			}
		}
	}

	*t2=*ddest;
	rtl_update_ZFSF(s,t2,id_dest->width);

   // if(cpu.pc==0x0012e24d) printf("2::dest:%x src1:%x src2:%x\n\n",*ddest,id_src1->val,id_src2->val);
}























static inline def_EHelper(rol){
  
    *t2=255;
  rtl_and(s,t2,t2,&id_src1->val);
  int temp=*t2;
// printf("1:rol,id_dest:%x temp:%d\n",*ddest,temp);

  
  while(temp>0){
   rtl_mv(s,t1,ddest);
   rtl_msb(s,t1,t1,id_dest->width);
   rtl_set_CF(s,t1);

   *t2=2;
   rtl_mv(s,t1,ddest);
   rtl_mul_lo(s,t1,t1,t2);
   rtl_addi(s,t1,t1,cpu.CF);
   rtl_mv(s,ddest,t1);
   
   temp=temp-1;
  }
 // printf("2:rol,id_dest:%x\n\n",*ddest);
}

static inline def_EHelper(ror){
	*t2=255;
	rtl_and(s,t2,t2,&id_src1->val);
   int temp=*t2;

   while(temp>0){
      rtl_mv(s,t1,ddest);
	  rtl_andi(s,t1,t1,1);
	  rtl_set_CF(s,t1);

	  *t2=2;
	  rtl_mv(s,t1,ddest);
	  rtl_div_q(s,t1,t1,t2);
      if(cpu.CF==1){
         int width=id_dest->width;
		 int num=1;
		 while(width>0){num*=2;}
         rtl_addi(s,t1,t1,cpu.CF);
         rtl_mv(s,ddest,t1);

	  }
	  else {
          rtl_mv(s,ddest,t1);
	  }

      temp--;
   }

}




static inline def_EHelper(setcc) {
//  if(cpu.pc==0x10013e) printf("setcc,edi:%x\n",cpu.edi);
  uint32_t cc = s->opcode & 0xf;//cc=subcode
  rtl_setcc(s, t1, cc);
  operand_write(s, id_dest, t1);

// if(cpu.pc==0x10013e) printf("setcc,edi:%x\n",cpu.edi);
  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}
