#ifndef __X86_RTL_H__
#define __X86_RTL_H__

#include <rtl/rtl.h>
#include "reg.h"

/* RTL pseudo instructions */

static inline def_rtl(lr, rtlreg_t* dest, int r, int width) {         //load reg to dest
  switch (width) {
    case 4: rtl_mv(s, dest, &reg_l(r)); return;
    case 1: rtl_host_lm(s, dest, &reg_b(r), 1); return;
    case 2: rtl_host_lm(s, dest, &reg_w(r), 2); return;
    default: assert(0);
  }
}

static inline def_rtl(sr, int r, const rtlreg_t* src1, int width) {  //send to reg
  switch (width) {
    case 4: rtl_mv(s, &reg_l(r), src1); return;//why case 1,2 rtl_host_sm,because rtl_mv parameter no width
    case 1: rtl_host_sm(s, &reg_b(r), src1, 1); return;
    case 2: rtl_host_sm(s, &reg_w(r), src1, 2); return;
    default: assert(0);
  }
}

static inline def_rtl(push, const rtlreg_t* src1) {
  // esp <- esp - 4
  // M[esp] <- src1
  rtl_subi(s,&cpu.esp,&cpu.esp,4);                         //Q:why esp? what the initial num of cpu.esp?    the IMAGE_START=0x10000,becuse 0-0x10000 used to store jmpc?
  rtl_sm(s,&cpu.esp,0,src1,4);
}

static inline def_rtl(pusha,const uint16_t* src1){
  //esp <- esp - 2
  //M[esp] <- src1
  rtl_subi(s,&cpu.esp,&cpu.esp,2);
  rtl_sm(s,&cpu.esp,0,(void*)src1,2);
}

static inline def_rtl(pop, rtlreg_t* dest) {
  // dest <- M[esp]
  // esp <- esp + 4

  rtl_lm(s,dest,&cpu.esp,0,4);
  rtl_addi(s,&cpu.esp,&cpu.esp,4);

}

static inline def_rtl(popa,uint16_t* dest){
   //dest <- M[esp]
   //esp <- esp + 2
   rtl_lm(s,(void *)dest,&cpu.esp,0,2);
   rtl_addi(s,&cpu.esp,&cpu.esp,2);
}



static inline def_rtl(is_sub_overflow, rtlreg_t* dest, const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 - src2)
  rtl_xor(s,t1,src1,src1);
  rtl_xor(s,t2,src1,res);
  rtl_and(s,t2,t1,t2);
  rtl_msb(s,t2,t2,width);

  rtl_mv(s,dest,t2);
}

static inline def_rtl(is_sub_carry, rtlreg_t* dest, const rtlreg_t* src1, const rtlreg_t* src2) {
  // dest <- is_carry(src1 - src2)
  rtl_sub(s,t3,src1,src2);
  rtl_setrelop(s,RELOP_LTU,dest,src1,t3);
}

static inline def_rtl(is_add_overflow, rtlreg_t* dest, const rtlreg_t* res, const rtlreg_t* src1, const rtlreg_t* src2, int width) {
  // dest <- is_overflow(src1 + src2)
  rtl_xor(s,t1,src1,src2);
  rtl_xor(s,t2,src1,res);
  rtl_not(s,t1,t1);
  rtl_and(s,t2,t1,t2);
  rtl_msb(s,t2,t2,width);

  rtl_mv(s,dest,t2);
}

static inline def_rtl(is_add_carry, rtlreg_t* dest, const rtlreg_t* res, const rtlreg_t* src1) {
  // dest <- is_carry(src1 + src2)
  // rtl_add(s,t3,res,src1);
   rtl_setrelop(s,RELOP_GTU,dest,src1,res);
}




#define def_rtl_setget_eflags(f) \
  static inline def_rtl(concat(set_, f), const rtlreg_t* src) { \
    cpu.f=*src; \
  } \
  static inline def_rtl(concat(get_, f), rtlreg_t* dest) { \
   *dest=cpu.f;  \
  }

def_rtl_setget_eflags(CF)
def_rtl_setget_eflags(OF)
def_rtl_setget_eflags(ZF)
def_rtl_setget_eflags(SF)

static inline def_rtl(update_ZF, const rtlreg_t* result, int width) { //the type of width is imm!(sword_t),while rtlreg_t is word_t
  // eflags.ZF <- is_zero(result[width * 8 - 1 .. 0])
  rtl_shli(s,t0,result,32-8*width);
  if(*t0==0)cpu.ZF=1;
  else cpu.ZF=0;
}

static inline def_rtl(update_SF, const rtlreg_t* result, int width) {
  // eflags.SF <- is_sign(result[width * 8 - 1 .. 0])
  rtl_msb(s,t0,result,width);
  cpu.SF=*t0;
}

static inline def_rtl(update_ZFSF, const rtlreg_t* result, int width) {
  rtl_update_ZF(s, result, width);
  rtl_update_SF(s, result, width);
}

#endif
