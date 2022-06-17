#include <am.h>
#include <nemu.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
	int val=inl(VGACTL_ADDR);
    int	w=val>>16;
    int h=val&0xffff;

  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = w, .height = h,
    .vmemsz = 0
  };
}



static inline int min(int x,int y){return (x<y)? x:y;}

void* memcpy1(void* out, const void* in, size_t n) {
    // assert((out!=NULL)&&(in!=NULL));
	 void *ret=out;
	 while(n--){
        *(char*)out=*(char*)in;
		 out=(char*)out+1;
		 in=(char*)in+1;
	 }
	 return ret;
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  int x=ctl->x,y=ctl->y,w=ctl->w,h=ctl->h;
  int val=inl(VGACTL_ADDR);int W=val>>16;int H=val&0xffff;

  uint32_t *pixels=ctl->pixels;
  int cp_bytes=sizeof(uint32_t)*min(w,W-x);
  /* 
  uint32_t *fb=(uint32_t *)(uintptr_t)FB_ADDR;
  int i;
  for(i=0;i<w*h;i++) fb[i]=*pixels;
  */
  
  for(int j=0;j<h && y+j<H;j++){
     memcpy1((uint32_t *)(uintptr_t)FB_ADDR+(y+j)*W+x,pixels,cp_bytes);
	 pixels+=w;
  }
  

  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}





void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
