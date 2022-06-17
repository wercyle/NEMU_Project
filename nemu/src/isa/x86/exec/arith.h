#include "cc.h"


//note:rtl_update_ZFSF use t0 to set ZF,SF.so if u use t0 to store result,after rtl_update_ZFSF,u will lost the result


static inline def_EHelper(add) {


    rtl_add(s,t3,&id_dest->val,&id_src1->val);
	operand_write(s,id_dest,t3);
	rtl_update_ZFSF(s,t3,id_dest->width);

	rtl_setrelop(s,RELOP_GTU,t1,&id_dest->val,t3);//judge unsigned up over flow(CF)
	rtl_set_CF(s,t1);

    rtl_xor(s,t1,&id_dest->val,&id_src1->val);//judge signed up and down overflow(OF)
	rtl_xor(s,t2,&id_dest->val,t3);
	rtl_not(s,t1,t1);
	rtl_and(s,t2,t1,t2);
	rtl_msb(s,t2,t2,id_dest->width);
	rtl_set_OF(s,t2);

    print_asm_template2(add);
}

static inline def_EHelper(sub) {
  
	rtl_sub(s,t3,&id_dest->val,&id_src1->val);
	operand_write(s,id_dest,t3);
	rtl_update_ZFSF(s,t3,id_dest->width);

    
	rtl_setrelop(s,RELOP_LTU,t1,&id_dest->val,t3);//judge unsigned down overflow(CF)
	rtl_set_CF(s,t1);


    rtl_xor(s,t1,&id_dest->val,&id_src1->val);//judge signed up and down overflow(OF)
	rtl_xor(s,t2,&id_dest->val,t3);
	rtl_and(s,t2,t1,t2);
	rtl_msb(s,t2,t2,id_dest->width);
	rtl_set_OF(s,t2);

    print_asm_template2(sub);

}

static inline def_EHelper(adc) {
  rtl_get_CF(s, s0);
  rtl_add(s, s0, dsrc1, s0);  //s0=src1+CF
  rtl_add(s, s1, ddest, s0);  //s1=dest+s0=dest+src1+CF
  rtl_update_ZFSF(s, s1, id_dest->width); //update ZF and SF,s1 is result

  rtl_is_add_overflow(s, s2, s1, ddest, dsrc1, id_dest->width); //s2=is_overflow(dest+src1)
  rtl_set_OF(s, s2);//OF=s2
  if (id_dest->width != 4) {
    rtl_andi(s, s1, s1, 0xffffffffu >> ((4 - id_dest->width) * 8));
  }

  rtl_is_add_carry(s, s2, s1, s0);
  rtl_is_add_carry(s, s0, s0, dsrc1);
  rtl_or(s, s0, s0, s2);
  rtl_set_CF(s, s0);//CF=s0

  operand_write(s, id_dest, s1);
  print_asm_template2(adc);
}

static inline def_EHelper(sbb) {
  rtl_get_CF(s, s0);
  rtl_add(s, s0, dsrc1, s0); //s0=src1+CF
  rtl_sub(s, s1, ddest, s0); //s1=dest-s0=dest-src1-CF
  rtl_update_ZFSF(s, s1, id_dest->width); //update ZF and SF

  rtl_is_sub_overflow(s, s2, s1, ddest, dsrc1, id_dest->width); //s2=is_overflow(dest-src1)
  rtl_set_OF(s, s2); //OF=s2

  rtl_is_add_carry(s, s2, s0, dsrc1);
  rtl_is_sub_carry(s, s0, ddest, s0);
  rtl_or(s, s0, s0, s2);
  rtl_set_CF(s, s0); //CF=s0

  operand_write(s, id_dest, s1);
  print_asm_template2(sbb);
}



static inline def_EHelper(cmp) { //dont change the result ,only change the CFLAGS
 // if(cpu.pc==0x10013c) printf("cmp,edi:%x\n",cpu.edi);

  rtl_sub(s,t3,&id_dest->val,&id_src1->val);
  rtl_update_ZFSF(s,t3,id_dest->width);

  rtl_setrelop(s,RELOP_LTU,t1,&id_dest->val,t3); //judge unsigned down overflow
  rtl_set_CF(s,t1);

  rtl_xor(s,t1,&id_dest->val,&id_src1->val);//judge signed up and down overflow
  rtl_xor(s,t2,&id_dest->val,t3);
  rtl_and(s,t2,t1,t2);
  rtl_msb(s,t2,t2,id_dest->width);
  rtl_set_OF(s,t2);

 // if(cpu.pc==0x1001fc) printf("cmp,dest:%x\n",id_dest->val);
 // if(cpu.pc==0x10013c) printf("cmp,edi:%x\n",cpu.edi);
  print_asm_template2(cmp);
}

static inline def_EHelper(inc) {
  rtl_addi(s,t3,&id_dest->val,1);
  operand_write(s,id_dest,t3);
  rtl_update_ZFSF(s,t3,id_dest->width);
  //why not rtl_addi(s,ddest,ddest,1)?so we can save the step(operand_write).
  //1:we need t3 to update EFLAGS
  //2:if we directly put num in ddest,then we could only put in 32byte(cause rtlreg_t* ddest)
  //,if we use operand_write,then we could put in based on the id_dest->width(8/16/32)

  rtl_xor(s,t1,&id_dest->val,&id_src1->val);
  rtl_xor(s,t2,&id_dest->val,t3);
  rtl_not(s,t1,t1);
  rtl_and(s,t2,t1,t2);
  rtl_msb(s,t2,t2,id_dest->width);
  rtl_set_OF(s,t2);

  print_asm_template1(inc);
}

static inline def_EHelper(dec) {


  rtl_subi(s,t3,&id_dest->val,1);
  operand_write(s,id_dest,t3);
  rtl_update_ZFSF(s,t3,id_dest->width);

   rtl_xor(s,t1,&id_dest->val,&id_src1->val);
   rtl_xor(s,t2,&id_dest->val,t3);
   rtl_and(s,t2,t1,t2);
   rtl_msb(s,t2,t2,id_dest->width);
   rtl_set_OF(s,t2);

 

  print_asm_template1(dec);
}

static inline def_EHelper(neg) {
  if(!id_dest->val){
     rtl_set_CF(s,rz);
  }
  else {//if neg!=0, unsigned overflow
     rtl_addi(s,t1,rz,1);
	 rtl_set_CF(s,t1);
  }
  rtl_add(s,t1,rz,&id_dest->val);
  *t1=-*t1;
  operand_write(s,id_dest,t1);
  rtl_update_ZFSF(s,t1,id_dest->width);

   rtl_xor(s,t2,&id_dest->val,&id_src1->val);
   rtl_xor(s,t1,&id_dest->val,t1);
   rtl_and(s,t2,t1,t2);
   rtl_msb(s,t2,t2,id_dest->width);
   rtl_set_OF(s,t2);


  print_asm_template1(neg);
}

static inline def_EHelper(mul) {
  rtl_lr(s, s0, R_EAX, id_dest->width);  //s0=eax or ax or al
  rtl_mul_lo(s, s1, ddest, s0);    //s1=dest*s0(eax,ax,al)

  switch (id_dest->width) {
    case 1://s0=al
      rtl_sr(s, R_AX, s1, 2);
      break;
    case 2://s0=ax
      rtl_sr(s, R_AX, s1, 2); //lower 16bit to ax
      rtl_shri(s, s1, s1, 16);//shift right 16 bit
      rtl_sr(s, R_DX, s1, 2); //higher 16bit to dx
      break;
    case 4://s0=eax
      rtl_mul_hi(s, s0, ddest, s0);//s0=((uint64_t)s0*(uint64_t)dest)>>32
      rtl_sr(s, R_EDX, s0, 4);//higher 32bit to edx
      rtl_sr(s, R_EAX, s1, 4);//lower 32bit to eax
      break;
    default: assert(0);
  }

  print_asm_template1(mul);
}

// imul with one operand
static inline def_EHelper(imul1) {
  switch (id_dest->width) {
    case 1:
      rtl_lr(s, s0, R_EAX, 1);//s0=al
      rtl_imul_lo(s, s1, ddest, s0);//s1=(sword)dest*(sbyte)s0,s1=dest*al

      rtl_sr(s, R_AX, s1, 2);
      break;
    case 2:
      rtl_lr(s, s0, R_EAX, 2);//s0=ax
      rtl_imul_lo(s, s1, ddest, s0);//s1=(sword)dest*(sword)s0,s1=dest*ax

      rtl_sr(s, R_AX, s1, 2);
      rtl_shri(s, s1, s1, 16);
      rtl_sr(s, R_DX, s1, 2);
      break;
    case 4:
      ;rtlreg_t *pdest = ddest;
      if (ddest == &cpu.edx) {
        rtl_mv(s, s0, ddest);
        pdest = s0;
      }
	  //what is the meaning of upper section
      rtl_imul_hi(s, &cpu.edx, pdest, &cpu.eax);//higher 32bit to edx
      rtl_imul_lo(s, &cpu.eax, pdest, &cpu.eax);//lower 32bit to eax
      break;
    default: assert(0);
  }

  print_asm_template1(imul);
}

// imul with two operands
static inline def_EHelper(imul2) {
  rtl_sext(s, dsrc1, dsrc1, id_src1->width);
  rtl_sext(s, ddest, ddest, id_dest->width);

  rtl_imul_lo(s, ddest, ddest, dsrc1);
  operand_write(s, id_dest, ddest);

  print_asm_template2(imul);
}

// imul with three operands
static inline def_EHelper(imul3) {
  rtl_sext(s, dsrc1, dsrc1, id_dest->width);
  rtl_sext(s, dsrc2, dsrc2, id_dest->width);

  rtl_imul_lo(s, ddest, dsrc2, dsrc1);
  operand_write(s, id_dest, ddest);

  print_asm_template3(imul);
}




static inline def_EHelper(div) {
  switch (id_dest->width) {
    case 1:
      rtl_lr(s, s0, R_AX, 2);
      rtl_div_q(s, s1, s0, ddest);
      rtl_sr(s, R_AL, s1, 1);
      rtl_div_r(s, s1, s0, ddest);
      rtl_sr(s, R_AH, s1, 1);
      break;
    case 2:
      rtl_lr(s, s0, R_AX, 2);
      rtl_lr(s, s1, R_DX, 2);
      rtl_shli(s, s1, s1, 16);
      rtl_or(s, s0, s0, s1);
      rtl_div_q(s, s1, s0, ddest);
      rtl_sr(s, R_AX, s1, 2);
      rtl_div_r(s, s1, s0, ddest);
      rtl_sr(s, R_DX, s1, 2);
      break;
    case 4:
      ; rtlreg_t *pdest = ddest;
      if (ddest == &cpu.eax) pdest = s0;
      rtl_mv(s, s0, &cpu.eax);
      rtl_div64_q(s, &cpu.eax, &cpu.edx, s0, pdest);
      rtl_div64_r(s, &cpu.edx, &cpu.edx, s0, pdest);
      break;
    default: assert(0);
  }

  print_asm_template1(div);
}

static inline def_EHelper(idiv) {
  switch (id_dest->width) {
    case 1:
      rtl_lr(s, s0, R_AX, 2);
      rtl_idiv_q(s, s1, s0, ddest);
      rtl_sr(s, R_AL, s1, 1);
      rtl_idiv_r(s, s1, s0, ddest);
      rtl_sr(s, R_AH, s1, 1);
      break;
    case 2:
      rtl_lr(s, s0, R_AX, 2);
      rtl_lr(s, s1, R_DX, 2);
      rtl_shli(s, s1, s1, 16);
      rtl_or(s, s0, s0, s1);
      rtl_idiv_q(s, s1, s0, ddest);
      rtl_sr(s, R_AX, s1, 2);
      rtl_idiv_r(s, s1, s0, ddest);
      rtl_sr(s, R_DX, s1, 2);
      break;
    case 4:
      ; rtlreg_t *pdest = ddest;
      if (ddest == &cpu.eax) pdest = s0;
      rtl_mv(s, s0, &cpu.eax);
      rtl_idiv64_q(s, &cpu.eax, &cpu.edx, s0, pdest);
      rtl_idiv64_r(s, &cpu.edx, &cpu.edx, s0, pdest);
      break;
    default: assert(0);
  }

  print_asm_template1(idiv);
}
