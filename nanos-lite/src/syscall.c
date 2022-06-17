#include <common.h>
#include <memory.h>
#include <fs.h>
#include <proc.h>
#include "syscall.h"
#include <sys/time.h>

static bool ex_flag=false;

//static char* empty_args[]={NULL};

static char* argv[]={"cat",NULL};
static char* envp[]={"/bin/",NULL};
static inline void sys_exit(Context *c){
  // halt(0);
      if(ex_flag==true){
			  context_uload(current,"/bin/nterm",argv,envp);
			  current->max_brk=0;
			  switch_boot_pcb();
			  yield();
	  }
	  else halt(c->GPR2);
}


static inline uintptr_t sys_write(uintptr_t fd,uintptr_t buf,uintptr_t len){
    return fs_write(fd,(void*)buf,len);	
}

static inline uintptr_t sys_read(uintptr_t fd,uintptr_t buf,uintptr_t len){
    return fs_read(fd,(void*)buf,len);
}

static inline uintptr_t sys_open(uintptr_t pathname,uintptr_t flags,uintptr_t mode){return fs_open((char*)pathname,flags,mode);}

static inline uintptr_t sys_close(uintptr_t fd){return fs_close(fd);}

static inline uintptr_t sys_lseek(uintptr_t fd,uintptr_t offset,uintptr_t whence){return fs_lseek(fd,offset,whence);}

uintptr_t sys_gettimeofday(struct timeval *tv,struct timezone *tz){
		  tv->tv_sec=io_read(AM_TIMER_UPTIME).us/1000000;
		  tv->tv_usec=io_read(AM_TIMER_UPTIME).us%1000000;
		  return 0;
}

int sys_execve(char *filename,char* const argv[],char* const envp[]){
		int ret=fs_open(filename,0,0);

		if(ret==-1) return -2;
		else {  
                ex_flag=true;
				context_uload(current,filename,argv,envp);
				current->max_brk=0;
				switch_boot_pcb();
				yield();
				return 0;
		}
}



void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
	case SYS_exit:sys_exit(c);break;//0
	case SYS_yield:yield();c->GPRx=0;break;//1
	case SYS_open:c->GPRx=sys_open(a[1],a[2],a[3]);break;//2
	case SYS_write:c->GPRx=sys_write(a[1],a[2],a[3]);break;//4
	case SYS_read:c->GPRx=(int)sys_read(a[1],a[2],a[3]);break;//3
	case SYS_close:c->GPRx=(int)sys_close(a[1]);break;
	case SYS_lseek:c->GPRx=sys_lseek(a[1],a[2],a[3]);break;
	case SYS_brk:
	       if(current->max_brk<a[2]) current->max_brk=a[2];
	       c->GPRx=mm_brk(a[1]);
		   break;
	case SYS_execve:c->GPRx=sys_execve((char*)a[1],(char**)a[2],(char**)a[3]);break;
	case SYS_gettimeofday:c->GPRx=sys_gettimeofday((struct timeval*)a[1],(struct timezone*)a[2]);break; 
    default: panic("Unhandled syscall ID = %d", a[0]);
  
  }
}


