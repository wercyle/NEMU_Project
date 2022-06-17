#ifndef __RTL_PSEUDO_H__
#define __RTL_PSEUDO_H__

#ifndef __RTL_RTL_H__
#error "Should be only included by <rtl/rtl.h>"
#endif

/* RTL pseudo instructions */

static inline def_rtl(li, rtlreg_t* dest, const rtlreg_t imm) {      //lend imm to *dest
  rtl_addi(s, dest, rz, imm);
}

static inline def_rtl(mv, rtlreg_t* dest, const rtlreg_t *src1) {     //mov *src1  to *dest
  if (dest != src1) rtl_add(s, dest, src1, rz);
}

static inline def_rtl(not, rtlreg_t *dest, const rtlreg_t* src1) {    //an wei qu fan
  *dest=~*src1; 
}

static inline def_rtl(neg, rtlreg_t *dest, const rtlreg_t* src1) {
  // dest <- -src1
  TODO();
}

static inline def_rtl(msb, rtlreg_t* dest, const rtlreg_t* src1, int width) {   //most significant bit
  // dest <- src1[width * 8 - 1]
  rtl_shri(s,dest,src1,8*width-1);
}


static inline def_rtl(sext, rtlreg_t* dest, const rtlreg_t* src1, int width) {  //signal extension,  note:the width is width of src1 not dest!
  // dest <- signext(src1[(width * 8 - 1) .. 0])
  // so here comes the question:we dont know the width we are going to change? A:just 32!
  /*
  rtl_msb(s,t0,src1,width);
  if(*t0==1){
     if(width==1) {rtl_xor(s,t0,r4,r1);rtl_or(s,dest,src1,t0);}
	 if(width==2) {rtl_xor(s,t0,r4,r2);rtl_or(s,dest,src1,t0);}
  }
  else{
     rtl_mv(s,dest,src1);
  }
  */
  int32_t dm=(int32_t)*src1;
  dm<<= 32-(8*width);
  dm>>= 32-(8*width);
  *dest=dm;
}


static inline def_rtl(zext, rtlreg_t* dest, const rtlreg_t* src1, int width) {  //zero extension
  // dest <- zeroext(src1[(width * 8 - 1) .. 0])
  
}


static inline def_rtl(geti,rtlreg_t* dest,const rtlreg_t* src1, int i){ //src1[i]>>dest
	*t0=*src1;
	rtl_shri(s,t0,t0,i);
	rtl_andi(s,t0,t0,0x1);
	*dest=*t0;
}

static inline def_rtl(puti,rtlreg_t*dest,const rtlreg_t* src1,int i){  //dest[i]=src1
	if(*src1==1){
       *t0=1;
	   rtl_shli(s,t0,t0,i);
	   rtl_or(s,dest,dest,t0);
	}
	else if(*src1==0){
		*t0=1;
		rtl_shli(s,t0,t0,i);
		rtl_not(s,t0,t0);
		rtl_and(s,dest,dest,t0);
	}
}











#endif
