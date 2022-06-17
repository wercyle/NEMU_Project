#include <proc.h>
#include <elf.h>
#include <fs.h>

#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

#define FILE_SYSTEM

#define ELF_OFFSET_IN_DISK 0

extern size_t ramdisk_read(void *buf,size_t offset,size_t len);
extern size_t get_ramdisk_size();

static inline void* pg_alloc2(int n){
		assert(n%PGSIZE==0);
		void *p=new_page(n/PGSIZE);
		memset(p,0,n);
		return p;
}


 uintptr_t loader(PCB *pcb, const char *filename) {

   Elf_Ehdr *elf;
   Elf_Phdr *ph=NULL;
   uint8_t buf[4096];
   
#ifdef FILE_SYSTEM
   int fd=fs_open(filename,0,0);//find ELF in file_table
   fs_read(fd,buf,4096);//put ELF Header and Program Header Table to buf
   fs_close(fd);
#else 
   ramdisk_read(buf,ELF_OFFSET_IN_DISK,4096);//put ELF Header and Program Header Table to buf
#endif
   elf=(void*)buf;


   /*fix the magic number with correct one*/
   const uint32_t elf_magic=0x464c457f;
   uint32_t *p_magic=(void*)buf;
   assert(*p_magic==elf_magic);


   /*Load each program segment*/
   int i=0;
   ph=(void *)(buf+elf->e_phoff);
   for(i=0;i<elf->e_phnum;i++){//scan the program table,load each segment into memoty*/
	   if(ph->p_type==PT_LOAD){
#ifndef HAS_VME
		   //read the content of the segment from the ELF file to memory region[VirtADDr,VirtAddr+FileSIz]
#ifdef FILE_SYSTEM
		   fs_lseek(fd,ph->p_offset,SEEK_SET);
		   fs_read(fd,(void*)(ph->p_vaddr),ph->p_filesz);//if def file system,then elf offset in disk is not 0
		   fs_close(fd);
#else
		   ramdisk_read((void*)(ph->p_vaddr),ph->p_offset,ph->p_filesz);
#endif
		   //zero the memory region [VirtAddr+FileSiz,VirtAddr+MemSiz]
		   memset((void*)(ph->p_vaddr+ph->p_filesz),0,ph->p_memsz-ph->p_filesz);
#endif

#ifdef HAS_VME
           //----------some information about this program segment---------------// 
           uintptr_t VirtAddr=ph->p_vaddr;
		   size_t FileSiz=ph->p_filesz,MemSiz=ph->p_memsz;
		   size_t offset=ph->p_offset;

		   uintptr_t bss_addr=VirtAddr+FileSiz;
		   uintptr_t final_addr=VirtAddr+MemSiz;

		   //---------alloc,map,read a uncomplete page form VirtAddr&0xfff to the end of the page---------------------//
           uintptr_t * tep;
		   tep=(uintptr_t *)pg_alloc2(PGSIZE);
		   assert(((uintptr_t)tep&0xfff)==0);

		   map(&(pcb->as),(void*)(VirtAddr&0xfffff000),tep,0);//why is the end of the pa page, why not the start of the pa page
		   int remain_space=FileSiz;                 //remain_space needed to map
		   uint32_t current_loc=VirtAddr&0xfff;      //current_loc is relative to pa page
		   uint32_t current_len=0xfff-current_loc+1; //current_len is current_loc to the end of pa page
		   uint32_t loc_tep=current_loc;
		   if(current_len>remain_space) current_len=remain_space;
		   loc_tep+=current_len;                     //current_loc+current_len

           fs_lseek(fd,offset,SEEK_SET);
		   fs_read(fd,(char*)tep+current_loc,current_len);
		   fs_close(fd);

		   remain_space -=current_len;
		   offset+=current_len;
		   VirtAddr =(VirtAddr&0xfffff000)+0x1000;

		   //------------if FileSiz is larger than current_len,which means the space that not bigger than one page is not enough for the program
		   //------------,so we have to alloc,map,read other pages to room for the program
		   int sign=0;
		   while(remain_space){
				   sign=1;
				   assert(remain_space>0);
				   assert((VirtAddr&0xfff)==0);

				   tep=(uintptr_t*)pg_alloc2(PGSIZE);
				   assert(((uintptr_t)tep&0xfff)==0);

				   map(&pcb->as,(void*)VirtAddr,tep,0);
				   current_len=0x1000;
				   if(current_len>remain_space) current_len=remain_space;

				   fs_lseek(fd,offset,SEEK_SET);
				   fs_read(fd,(char*)tep,current_len);
				   fs_close(fd);

				   remain_space-=current_len;
				   offset+=current_len;
				   VirtAddr=(VirtAddr&0xfffff000)+0x1000;
		   }
		   if(sign==1)
				   assert((current_len&0xfff)==(bss_addr&0xfff));
		   else
				   assert((loc_tep&0xfff)==(bss_addr&0xfff));

			//----------------load the .bss---------------------------------------------------------------//
			remain_space=MemSiz-FileSiz;
			current_loc=bss_addr&0xfff;
			current_len=0xfff-current_loc+1;
			if(current_len>remain_space) current_len=remain_space;

			if((bss_addr&0xfff)==0){
					tep=(uintptr_t*)pg_alloc2(PGSIZE);
					assert(((uintptr_t)tep&0xfff)==0);
					map(&pcb->as,(void*)bss_addr,tep,0);
					current_loc+=current_len;
			}
			else current_loc+=current_len;

			remain_space-=current_len;
			bss_addr=(bss_addr&0xfffff000)+0x1000;



			sign=0;
			while(remain_space){
					sign=1;
					assert(remain_space>0);
					assert((bss_addr&0xfff)==0);

					tep=(uintptr_t*)pg_alloc2(PGSIZE);
					assert(((uintptr_t)tep&0xfff)==0);

					map(&pcb->as,(void*)bss_addr,tep,0);
					current_len=0x1000;
					if(current_len>remain_space) current_len=remain_space;

					remain_space-=current_len;
					bss_addr=(bss_addr&0xfffff000)+0x1000;
			}
			if(sign==1)
					assert((current_len&0xfff)==(final_addr&0xfff));
			else 
					assert((current_loc&0xfff)==(final_addr&0xfff));
	

#endif
		   ph++;
	   }
   }



   volatile uint32_t entry =elf->e_entry;
   return entry; 
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
   Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}


/*
 uintptr_t loader(PCB *pcb, const char *filename) {

   Elf_Ehdr *elf;
   Elf_Phdr *ph=NULL;
   uint8_t buf[4096];
   
#ifdef FILE_SYSTEM
   int fd=fs_open(filename,0,0);//find ELF in file_table
   fs_read(fd,buf,4096);//put ELF Header and Program Header Table to buf
   fs_close(fd);
#else 
   ramdisk_read(buf,ELF_OFFSET_IN_DISK,4096);//put ELF Header and Program Header Table to buf
#endif
   elf=(void*)buf;


   //fix the magic number with correct one
   const uint32_t elf_magic=0x464c457f;
   uint32_t *p_magic=(void*)buf;
   assert(*p_magic==elf_magic);


   //Load each program segment
   int i=0;
   ph=(void *)(buf+elf->e_phoff);
   for(i=0;i<elf->e_phnum;i++){//scan the program table,load each segment into memoty
	   if(ph->p_type==PT_LOAD){
		   //read the content of the segment from the ELF file to memory region[VirtADDr,VirtAddr+FileSIz]
#ifdef FILE_SYSTEM
		   fs_lseek(fd,ph->p_offset,SEEK_SET);
		   fs_read(fd,(void*)(ph->p_vaddr),ph->p_filesz);//if def file system,then elf offset in disk is not 0
		   fs_close(fd);
#else
		   ramdisk_read((void*)(ph->p_vaddr),ph->p_offset,ph->p_filesz);
#endif
		   //zero the memory region [VirtAddr+FileSiz,VirtAddr+MemSiz]
		   memset((void*)(ph->p_vaddr+ph->p_filesz),0,ph->p_memsz-ph->p_filesz);
		   ph++;
	   }
   }


   volatile uint32_t entry =elf->e_entry;
   return entry;

   
}
*/
