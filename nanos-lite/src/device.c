#include <common.h>
#include <fs.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)  //-----macro in amdev------NAME(ESCAPE) NAME(F1) NAME(F2)--------macro above--------[AM_KEY_ESCAPE]=ESCAPE,[AM_KEY_F1]=F1,[AM_KEY_F2]=F2
};



size_t serial_write(const void *buf, size_t offset, size_t len) {
	  for(size_t i=0;i<len;++i) putch(*(char*)(buf+i));
      return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
	  /*
      int keycode=io_read(AM_INPUT_KEYBRD).keycode;
	  bool keydown=io_read(AM_INPUT_KEYBRD).keydown;

	  if(keycode){
         printf("keydown %d\n",keydown);
         if(keydown){ sprintf(buf,"kd %s\n",keyname[keycode]);printf("pkd %s\n",keyname[keycode]);}
		 else {sprintf(buf,"ku %s\n",keyname[keycode]);printf("pku %s\n",keyname[keycode]);}
         return strlen(buf);
	  }

	  else return -1;
	  */
	  int l=-1;
	  AM_INPUT_KEYBRD_T ev =io_read(AM_INPUT_KEYBRD);

	  if(ev.keycode==AM_KEY_NONE){ 
		  l=sprintf(buf,"t %d\n",520);
	  }
	  else {
         if(ev.keydown){
			l=sprintf(buf,"kd %s\n",keyname[ev.keycode]);
		 }
	     else{
			l=sprintf(buf,"ku %s\n",keyname[ev.keycode]);
		 }
	}
//	printf("%s\n",(char*)buf);
	return l;
}


//dispinfo to buf
static char dispinfo[128] __attribute__((used));
size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  strncpy(buf,dispinfo+offset,len);
  Log("dispinfo_read: %s offset: %d len:%d",buf,offset,len);
  return len;
}

static int width;//initialize in init_device

size_t fb_write(const void *buf, size_t offset, size_t len) {

 // int width=io_read(AM_GPU_CONFIG).width;
 // int height=io_read(AM_GPU_CONFIG).height;
 // yield();

  int x=(offset/4)%width;
  int y=(offset/4)/width;

  io_write(AM_GPU_FBDRAW,x,y,(uint32_t*)buf,len/4,1,true); 
//  printf("io_write\n");

  return len;
}



void init_device() {
  Log("Initializing devices...");
  ioe_init();

  //get the screen size to dispinfo 
  width=io_read(AM_GPU_CONFIG).width;
  int height=io_read(AM_GPU_CONFIG).height; 

  int r=sprintf(dispinfo,"WIDTH:%d\nHEIGHT:%d\n",width,height);
  Log("dispinfo_read: %s %d",dispinfo,r);
}
