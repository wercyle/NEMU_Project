#include <memory.h>
#include <common.h>
#include <proc.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
   for(int i=0;i<nr_page;i++){
	  pf=(char*)pf+PGSIZE;
  }
   return pf;
}

static inline void* pg_alloc(int n) {
    assert(n%PGSIZE==0);
	void *p=new_page(n/PGSIZE);
	memset(p,0,n);
	return p;
}

void free_page(void *p) {
  panic("not implement yet");
}



//extern PCB * current;
/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  #ifdef HAS_VME
  if(brk>=current->max_brk){
		  uintptr_t now;
		  now=(current->max_brk)&0xfffff000;
		  void *p=pg_alloc(PGSIZE);
		  map(&current->as,(void*)now,p,0);
		  now=(now&0xfffff000)+0x1000;
		  current->max_brk=now;

		  while(brk>=current->max_brk){
				  p=pg_alloc(PGSIZE);
				  map(&current->as,(void*)now,p,0);
				  now=(now&0xfffff000)+0x1000;
				  current->max_brk=now;
		  }
  }
#endif

  return 0;
}

void init_mm() {
  //1 set the starting address of the heap the TRM provided as the starting address of the free pages,
  //then the system can alloc free physical pages through the new_page function;
  pf = (void *)ROUNDUP(heap.start, PGSIZE); //heap.start maybe == 0x78f000,so pf =0x78f000
  Log("free physical pages starting from %p", pf);

  //2 set the page alloc and free function;fill page dir ans page tbl in kas(kernel virtual address space);set cr3 and cr0
#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
