#include <isa.h>
#include <memory/vaddr.h>
#include <memory/paddr.h>
paddr_t isa_mmu_translate(vaddr_t vaddr, int type, int len) {
		uint32_t *loc;
		loc = (uint32_t*)guest_to_host(cpu.cr3);
		loc = loc+((vaddr & ~0x3fffff)>>22); // loc+DIR,loc is the head address of Page Dir
		assert(loc != NULL);
		if( ((*loc)&0xfff)!=1 ){
				//printf ("loc:%p\n",(intptr_t)loc);
				printf("cr3:%x vaddr:%x *loc:%x\n",cpu.cr3,vaddr,*loc);
				assert(0);
		}
		assert((*loc)!= 0);


		uint32_t *loc_pt;
		loc_pt = (uint32_t*)guest_to_host((*loc)&0xfffff000);
		loc_pt = loc_pt+((vaddr & 0x003ff000)>>12); //loc_pt+PAGE,loc_pt is the head address of Page Table
		assert(loc_pt != NULL);
		if( ((*loc_pt)&0xfff)!=1 ){
				printf("cr3:%x vaddr:%x loc_pt:%x\n",cpu.cr3,vaddr,*loc_pt);
				assert(0);
		}
		assert((*loc_pt)!=0);


		uintptr_t pa;
		pa = ((*loc_pt)&0xfffff000);
		paddr_t real_pa = pa +(vaddr & 0xfff);  //pa+OFFSET,pa is the head address of Page Frame


		if(((real_pa+len-1)&0xfffff000)!=pa) return MEM_RET_CROSS_PAGE; //means has cross the page
		else return real_pa;

        assert(vaddr==real_pa);
}
