#include <am.h>
#include <nemu.h>
#include <klib.h>

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};
//NEMU_PADDR_SPACE=[0,0x40000000]



#define USER_SPACE RANGE(0x40000000, 0xc0000000)



bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  //1:set the page alloc and free function,so the am can alloc and free physical pages using this two function
  //e.g.: the map function(am) can use pgalloc_usr and pgfree_f to alloc pages for page dir or page tbl
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  //2:1)alloc space for kas.ptr
  //  2)fill the page dir and page tbl in kas(kernel virtual address space ) through calling map
  kas.ptr = pgalloc_f(PGSIZE);

  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;
    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, 0);
    }
  }

  //3:set cr3 and cr0
  set_cr3(kas.ptr);
  set_cr0(get_cr0() | CR0_PG);
  vme_enable = 1;


  return true;
}




//creat the default address space 
void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

//destory the specified address space
void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  c->cr3 = (vme_enable ? (void *)get_cr3() : NULL);
}

void __am_switch(Context *c) {
  if (vme_enable && c->cr3 != NULL) {
    set_cr3(c->cr3);
  }
}




// map the va(virtual address ) page in as   to    pa(paddr address) page
//fill the page dir and page tbl in kas(kernel virtual address space ) through calling map
void map(AddrSpace *as, void *va, void *pa, int prot) {
		assert(as!=NULL);

		uintptr_t *loc;
		loc=(uintptr_t*)as->ptr; // now the loc point to the base address of the page directory
		if((uintptr_t)loc % PGSIZE != 0){
				printf("%p\n",loc);
				assert(0);
		}

		uintptr_t src = (uintptr_t )va;
		assert(src % PGSIZE==0);
		uintptr_t dst = (uintptr_t )pa;
		assert(dst % PGSIZE==0);


        loc=loc+((src & ~0x3fffff)>>22);//page tbl entry in page dir;;;;(src & ~0x3fffff) is DIR field in linear address
		uintptr_t* loc_pt;//page table's location 

		if(*loc == 0 ){ //means this page tbl in page dir has not alloc yet,now needed created
				uintptr_t *tep;
				tep=(uintptr_t*)pgalloc_usr(PGSIZE);//creat this page tbl
				assert(((uintptr_t)tep & 0xfff) == 0);
				loc_pt=tep;

				*loc= (((uintptr_t)tep)|1);//fill the page tbl entry in page dir
		}
		else { //means this page tbl in page dir is exited ,so we can directly get the address of the page table's location
				 loc_pt=(uintptr_t*)((*loc)&0xfffff000);
				 //page frame address in page tbl entry is used;
		}


		loc_pt=loc_pt + ((src & 0x003ff000)>>12);//page frame entry in page tbl;;;;(src & 0x003ff000) is PAGE field in linear address

		if(*loc_pt == 0){ //means this page in page tbl has not mapped
				*loc_pt = (dst|1); // fill the page frame entry  in page tbl;
		}
		else{ //means this page in page tbl is mapped already
			//	assert(*loc_pt==(dst|1));
		}



}




Context* ucontext(AddrSpace *as, Area kstack, void *entry) {
   Context* c=(Context*)kstack.end -1;
   c->cr3=(void*)as->ptr;
   c->edi=0,c->esi=0,c->ebp=0,c->esp=0;
   c->ebx=0,c->edx=0,c->ecx=0,c->eax=0;
   c->irq=0x81;
   c->esp=(uintptr_t)kstack.end;
   //c->esp=(uintptr_t)((uintptr_t*)heap.end-1);
   //c->eax=(uintptr_t)((uintptr_t*)heap.end-1);
   c->pc=(uintptr_t)(entry);
   c->cs=USEL(3);
   c->eflags=0x00000200;
   c->esp3=0;c->ss3=0;

   return c;
}
