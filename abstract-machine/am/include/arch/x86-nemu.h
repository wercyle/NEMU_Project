#ifndef ARCH_H__
#define ARCH_H__
/*
struct Context {
  uintptr_t esi, ebx, eax, eip, edx, eflags, ecx, cs, esp, edi, ebp;
  void *cr3;
  int irq;
};
*/
/*
struct Context{
  uintptr_t eflags,cs,pc;
  int irq;
  uintptr_t eax,ecx,edx,ebx,esp,ebp,esi,edi;
  void *cr3;

};
*/

struct Context{
   void *cr3;
   uintptr_t edi,esi,ebp,esp,ebx,edx,ecx,eax;
   int irq;
   uintptr_t pc,cs,eflags;
   uintptr_t esp3,ss3;

};


#define GPR1 eax
#define GPR2 ebx
#define GPR3 ecx
#define GPR4 edx
#define GPRx eax


/*
#define GPR1 eax
#define GPR2 ebx
#define GPR3 ecx
#define GPR4 edx
#define GPRx eax
*/



#endif
