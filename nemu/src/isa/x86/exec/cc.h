#ifndef __CC_H__
#define __CC_H__

#include "../local-include/rtl.h"

enum {
  CC_O, CC_NO, CC_B,  CC_NB,
  CC_E, CC_NE, CC_BE, CC_NBE,
  CC_S, CC_NS, CC_P,  CC_NP,
  CC_L, CC_NL, CC_LE, CC_NLE
};


/* Condition Code */

 static const char *cc_name[] = {
    "o", "no", "b", "nb",
    "e", "ne", "be", "nbe",
    "s", "ns", "p", "np",
    "l", "nl", "le", "nle"
  };

static inline const char* get_cc_name(int subcode) {
   return cc_name[subcode];
}

static inline void rtl_setcc(DecodeExecState *s, rtlreg_t* dest, uint32_t subcode) {
  uint32_t invert = subcode & 0x1;

  // TODO: Query EFLAGS to determine whether the condition code is satisfied.
  // dest <- ( cc is satisfied ? 1 : 0)
  switch (subcode & 0xe) {//dont care last 
    case CC_O:if(cpu.OF==1) *dest=1;else *dest=0;break;
    case CC_B:if(cpu.CF==1) *dest=1;else *dest=0;break;
    case CC_E:if(cpu.ZF==1) *dest=1;else *dest=0;break;
    case CC_BE:if(cpu.CF==1||cpu.ZF==1) *dest=1;else *dest=0;break;
    case CC_S:if(cpu.SF==1) *dest=1;else *dest=0;break;
    case CC_L:if(cpu.SF!=cpu.OF) *dest=1;else *dest=0;break;
    case CC_LE:if((cpu.SF!=cpu.OF)||(cpu.ZF==1)) *dest=1;else *dest=0;break; 
    case CC_P: panic("PF is not supported");break;
    default: panic("should not reach here");break;
  }

  if (invert) { //care last,if is odd,then qufan,if is even dest stay unchanged   
	  //why we can do in this way?
	  //if u read the chart,u will find the EFLAGS requirement of odd is opsite to even(odd-1)
	  //for example:90(OF=1),91(OF=0);92(CF=1),93(CF=0)....
    rtl_xori(s, dest, dest, 0x1);
  }
  assert(*dest == 0 || *dest == 1);
}

#endif
