#include <am.h>
#include <x86.h>
#include <klib.h>

#define NR_IRQ         256     // IDT size
#define SEG_KCODE      1
#define SEG_KDATA      2

static Context* (*user_handler)(Event, Context*) = NULL; //static function pointer
//Q:who provide the call back function
//A:nanos-lite,irq.c
//Q:when the pointer user_handler being init?
//A:in the cte_init,the user_handler=handler(which is parameter)
//Q:when the user_handler being used?
//A:in the __am_irq_handle

void __am_irq0();
void __am_vecsys();
void __am_vectrap();
void __am_vecnull();



Context* __am_irq_handle(Context *c) {
  __am_get_cur_as(c);

  if (user_handler) {
    Event ev = {0};

    switch (c->irq) {
	  case 32:  ev.event = EVENT_IRQ_TIMER;break;//EVENT_IRQ_TIMER=5;
	  case 0x80:ev.event = EVENT_SYSCALL;break;//EVENT_SYSCALL=2;
	  case 0x81:ev.event = EVENT_YIELD;break;//EVENT_YIELD=1;
      default: ev.event = EVENT_ERROR; break;//EVEVT_ERROR=4;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }

  __am_switch(c);

  return c;
}

/* Gate descriptors for interrupts and traps
typedef struct {
  uint32_t off_15_0  : 16; // Low 16 bits of offset in segment
  uint32_t cs        : 16; // Code segment selector
  uint32_t args      :  5; // # args, 0 for interrupt/trap gates
  uint32_t rsv1      :  3; // Reserved(should be zero I guess)
  uint32_t type      :  4; // Type(STS_{TG,IG32,TG32})
  //STS_TG:segment type of system:trap gate
  //STS_IG:segment type of system:interrupts gate
  //STS_T32A:segment type of system:Available TSS(Task State Segment)

  uint32_t s         :  1; // Must be 0 (system)
  uint32_t dpl       :  2; // Descriptor(meaning new) privilege level
  uint32_t p         :  1; // Present
  uint32_t off_31_16 : 16; // High bits of offset in segment
} GateDesc32;


#define GATE32(type, cs, entry, dpl) (GateDesc32)              \
  {  (uint32_t)(entry) & 0xffff, (cs), 0, 0, (type), 0, (dpl), \
  1, (uint32_t)(entry) >> 16 }
*/



/*Segment Descriptor
typedef struct {
  uint32_t lim_15_0   : 16; // Low bits of segment limit
  uint32_t base_15_0  : 16; // Low bits of segment base address
  uint32_t base_23_16 :  8; // Middle bits of segment base address
  uint32_t type       :  4; // Segment type (see STS_ constants)
  uint32_t s          :  1; // 0 = system, 1 = application
  uint32_t dpl        :  2; // Descriptor Privilege Level
  uint32_t p          :  1; // Present
  uint32_t lim_19_16  :  4; // High bits of segment limit
  uint32_t avl        :  1; // Unused (available for software use)
  uint32_t l          :  1; // 64-bit segment
  uint32_t db         :  1; // 32-bit segment
  uint32_t g          :  1; // Granularity: limit scaled by 4K when set
  uint32_t base_31_24 :  8; // High bits of segment base address
} SegDesc;


#define SEG32(type, base, lim, dpl) (SegDesc)         \
{ ((lim) >> 12) & 0xffff, (uintptr_t)(base) & 0xffff, \
  ((uintptr_t)(base) >> 16) & 0xff, type, 1, dpl, 1,  \
  (uintptr_t)(lim) >> 28, 0, 0, 1, 1, (uintptr_t)(base) >> 24 }
*/


//initialize GDT
  #define NR_SEG 6
  static SegDesc gdt[NR_SEG]={};
  static TSS32 tss={};



bool cte_init(Context*(*handler)(Event, Context*)) {
  static GateDesc32 idt[NR_IRQ];

  // initialize IDT
  for (unsigned int i = 0; i < NR_IRQ; i ++) {
    idt[i]  = GATE32(STS_TG, KSEL(SEG_KCODE), __am_vecnull, DPL_KERN);
	//SYStem Segment type bit      cs               entry       dpl
  }

  // -----------------------Interrupt Gate  ----------------------------
  idt[32]   = GATE32(STS_IG, KSEL(SEG_KCODE), __am_irq0,    DPL_KERN); //EVENT_IRQ_TIMER
  // ---------------------- Trap Gate ----------------------------
  idt[0x80] = GATE32(STS_TG, KSEL(SEG_KCODE), __am_vecsys,  DPL_USER); //EVENT_SYSCALL
  idt[0x81] = GATE32(STS_TG, KSEL(SEG_KCODE), __am_vectrap, DPL_KERN); //EVENT_YIELD
 
  //sizeof(idt)=2048
  //8byte*256=2048!
  set_idt(idt, sizeof(idt));





  gdt[1]=SEG32(STA_X|STA_R,      0,   0xffffffff,DPL_KERN);
// Application Segment type bit  base       lim      dpl
  gdt[2]=SEG32(STA_W      ,      0,   0xffffffff,DPL_KERN);
  gdt[3]=SEG32(STA_X|STA_R,      0,   0xffffffff,DPL_USER);
  gdt[4]=SEG32(STA_W      ,      0,   0xffffffff,DPL_USER);
  gdt[5]=SEG32(STS_T32A   ,   &tss, sizeof(tss)-1,DPL_KERN);

  set_gdt(gdt,sizeof(gdt[0])*NR_SEG);

  //initialize TSS
  tss.ss0=KSEL(2);
  set_tr(KSEL(5));
  //printf("%d",tss.esp0);




  // register event handler
  user_handler = handler;

  return true;
}


Context* kcontext(Area kstack, void (*entry)(void *), void *arg) {
//kstack is range of the stack; entry is the entrance of the kernel thread;
  Context *c =(Context*)kstack.end-2;
  intptr_t* loc;
  loc=(intptr_t*)(&(c->ss3)+2);
  *loc=(intptr_t)arg;

  c->cr3=0;
  c->edi=0;c->esi=0;c->ebp=0;c->esp=0;
  c->ebx=0;c->edx=0;c->ecx=0;c->eax=0;
  c->irq=0x81;
  c->pc=(intptr_t)(entry);
  c->cs=KSEL(1);
  c->eflags=0x00000200;
  c->esp3=0;c->ss3=0;
  return c;
}

void yield() {
  asm volatile("int $0x81");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
