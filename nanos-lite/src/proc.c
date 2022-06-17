#include <proc.h>

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() { //to init the current point
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    if(j%1000==0) {Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (intptr_t)arg, j);}
   // Log("Hello World from Nanos-lite with arg '%p' for the %dth time!", (intptr_t)arg, j);
	j++;
  //  yield();
  }
}


//----------------------------------------CONTEXT_KLOAD!--------------------------------------------------
void context_kload(PCB* pcb,void *loc,void* arg){
		Area area;
		area.start=pcb;
		area.end=(char*)pcb+sizeof(PCB);
		pcb->cp=kcontext(area,loc,arg);
}


static char *argv2[15];
static char *envp2[15];

static char argv3[15][256];
static char envp3[15][256];


//---------------------------------------CONTEXT_ULOAD---------------------------------------------------
extern  uintptr_t loader(PCB *pcb, const char *filename);
void context_uload(PCB* pcb,const char* filename,char *const argv[],char *const envp[]){
     #ifdef HAS_VME
	    protect(&(pcb->as));//creat virtrual address space for user process
	 #endif

        uintptr_t *loc;
		loc=((uintptr_t*)new_page(8)-1);  //note:this physical address is guest address


     #ifdef HAS_VME
	    uintptr_t *loc_tep;//map the virtual user stack to the physical user stack
		loc_tep=loc+1;
		for(int i=1;i<=8;i++){ //why not <=8??????
				map(&(pcb->as),(char*)pcb->as.area.end-(i*0x1000),(char*)loc_tep-(i*0x1000),0);
		}
	 #endif


		assert(envp!=NULL);
		int env_argc=0;
		if(envp!=NULL){
				for(int i=0;;i++){
						if(envp[env_argc]==NULL){
								envp2[env_argc]=NULL;
								break;
						}
						else {
								strcpy(envp3[env_argc],envp[env_argc]);
								envp2[env_argc]=envp3[env_argc];
								env_argc++;
						}
				}
				for(int i=env_argc;i>=0;i--){
						*loc=(uintptr_t)envp2[i];
						 loc=loc-1;
				}
		}

		assert(argv!=NULL);
		int argc=0;
		if(argv!=NULL){
				for(int i=0;;i++){
						if(argv[argc]==NULL){
								argv2[argc]=NULL;
								break;
						}
						else{
								strcpy(argv3[argc],argv[argc]);
								argv2[argc]=argv3[argc];
								argc++;
						}
				}
				for(int i=argc;i>=0;i--){
						*loc = (uintptr_t)argv2[i];
						loc =loc-1;
				}
		}

		*loc = (uintptr_t)argc;

     #ifdef HAS_VME
	    uint32_t delta=loc_tep-loc;
		uintptr_t *st;
		st=(uintptr_t *)pcb->as.area.end;
		st=st-delta;
	 #endif

	 #ifndef HAS_VME
	    pcb->as.ptr=0;
	 #endif
	 


		Area Stack_area;
		Stack_area.start=pcb;
		Stack_area.end=(char*)pcb+sizeof(PCB);
		pcb->cp=ucontext(&(pcb->as),Stack_area,(void*)loader(pcb,filename)); //essential step,get the cp and as

     #ifdef HAS_VME
	    pcb->cp->GPRx=(uintptr_t)st;
	 #endif
     #ifndef HAS_VME
		pcb->cp->GPRx=(uintptr_t)loc;
	 #endif
}


static char *argv[]={"--skip",NULL};
static char *envp[]={"PATH/bin/:usr/bin",NULL};



void init_proc() {
 
  //int a=1;
  //context_kload(&pcb[0],hello_fun,&a);
  context_uload(&pcb[0],"/bin/hello",argv,envp);
  context_uload(&pcb[1],"/bin/nterm",argv,envp);

  switch_boot_pcb();    // to init the current pointer
  Log("Initializing processes....");

 // naive_uload(NULL,"/bin/nterm");
 // load program here

}

Context* schedule(Context *prev) {
   current->cp=prev;//save the context pointer
   current=(current==&pcb[0]?&pcb[1]:&pcb[0]);
  // current=&pcb[1];
   return current->cp;//then return the new context
}
