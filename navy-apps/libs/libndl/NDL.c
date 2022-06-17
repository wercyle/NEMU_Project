#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <NDL.h>
#include <sys/time.h>


static int has_nwm=0;
//static int evtdev = -1;
//static int fbdev = -1;
static FILE *fbdev,*evtdev;
static int screen_w = 0, screen_h = 0,canvas_w,canvas_h,pad_x,pad_y;
static uint32_t *canvas;
static void NDL_get_display_info();

static struct timeval now;

//-----------------------------------------time--------------------------------

uint32_t NDL_GetTicks() {
  gettimeofday(&now,NULL);
  return now.tv_sec*1000+now.tv_usec/1000;
}

//-------------------------------------------------------------------------------




//---------------------------------------keyboard---------------------------------
/*
int NDL_PollEvent(char *buf, int len) {

	assert(evtdev);
	char ch;
	char *p=buf;
	while((ch=fgetc(evtdev))!=-1){
       *p++=ch;
	   assert(p-buf<len);
	   if(ch=='\n'){*p='\0';break;}
	}

}
*/
#define keyname(k) #k, //macro2

//macro1+macro2
static const char *keys[]={
      "NONE",
	  _KEYS(keyname)

};

#define numkeys (sizeof(keys)/sizeof(keys[0]))

int NDL_PollEvent(NDL_Event* event){
    char buf[256],*p=buf,ch;
	assert(evtdev);

	while(1){

	while((ch=fgetc(evtdev))!=-1){
       *p++=ch;
	   assert(p-buf<sizeof(buf));
	   if(ch=='\n')break;
	}

	if(buf[0]=='k'){
        char keyname[32];
		event->type=buf[1]=='d'?NDL_EVENT_KEYDOWN:NDL_EVENT_KEYUP;   //event->type
		event->data=-1;                                              //event->data
		sscanf(buf+3,"%s",keyname);
        for(int i=0;i<numkeys;i++){
           if(strcmp(keys[i],keyname)==0){  //keys?numkeys?
              event->data=i;
			  break;
			}
		}
		assert(event->data>=1&&event->data<numkeys);
		return 1;
	}
	else if(buf[0]=='t'){
			int tsc;
			sscanf(buf+2,"%d",&tsc);
			event->type=NDL_EVENT_TIMER;
			event->data=tsc;
			return 1;
	}

	}
	//assert(0);
	return 0;
    
}





int NDL_WaitEvent(NDL_Event* event){
    char buf[256],*p=buf,ch;
	assert(evtdev);

	while(1){

	while((ch=fgetc(evtdev))!=-1){
       *p++=ch;
	   assert(p-buf<sizeof(buf));
	   if(ch=='\n')break;
	}

	if(buf[0]=='k'){
        char keyname[32];
		event->type=buf[1]=='d'?NDL_EVENT_KEYDOWN:NDL_EVENT_KEYUP;
		event->data=-1;
		sscanf(buf+3,"%s",keyname);
        for(int i=0;i<numkeys;i++){
           if(strcmp(keys[i],keyname)==0){  //keys?numkeys?
              event->data=i;
			  break;
			}
		}
		assert(event->data>=1&&event->data<numkeys);
		return 1;
	}

	if(buf[0]=='t'){
        int tsc;
		sscanf(buf+2,"%d",&tsc);
		event->type=NDL_EVENT_TIMER;
		event->data=tsc;
		return 1;

	}
	}
	assert(0);
	return 0;
    
}
//-------------------------------------------------------------------------------------------------






//---------------------------------------------video---------------------------------------------

void NDL_GetCanvasInfo(int *w,int *h){
		*w=canvas_w;
		*h=canvas_h;
}


void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
	
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }



  if(getenv("NWM_APP")){has_nwm=1;}
  else {has_nwm==0;}



  if (has_nwm){}
  else {
       NDL_get_display_info();
       if((*w)&&(*h)){
          canvas_w=*w;
          canvas_h=*h;
	   }
	   else {
		  canvas_w=screen_w;
		  canvas_h=screen_h;
	   }
       canvas=malloc(sizeof(uint32_t*)*canvas_w*canvas_h);
       // printf("cw:%d ch%d\n",canvas_w,canvas_h);
       assert(canvas);

	   assert(screen_w>=canvas_w);
	   assert(screen_h>=canvas_h);
	   pad_x=(screen_w-canvas_w)/2;
	   pad_y=(screen_h-canvas_h)/2;

  }
}


void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
	if(has_nwm){}
	else {
	/*	
       for(int i=0;i<h;i++){
          for(int j=0;j<w;j++){
             canvas[(y+i)*canvas_w+(x+j)]=pixels[i*w+j];
		  }
	   }
	  */ 
	   for(int j=0;j<h;j++){
			   memcpy(canvas+(y+j)*canvas_w+x,pixels,4*w);
			   pixels+=w;
	   }
	   
	}
}



int NDL_Render(){
    if(has_nwm){}
	else {
         for(int i=0;i<canvas_h;i++){
			 //printf("311\n");
             fseek(fbdev,((pad_y+i)*screen_w+pad_x)*sizeof(uint32_t),SEEK_SET);
             //printf("312\n");
			 fwrite(&canvas[i*canvas_w],sizeof(uint32_t),canvas_w,fbdev);
             //printf("313\n");
		}
	}
}





void NDL_get_display_info(){
    FILE *dispinfo=fopen("/proc/dispinfo","r");
	//assert(dispinfo);

	char buf[128],key[128],value[128],*delim;

	   fgets(buf,128,dispinfo);
       *(delim=strchr(buf,':'))='\0'; 
	   sscanf(buf,"%s",key);
	   sscanf(delim+1,"%s",value);
	   if(strcmp(key,"WIDTH")==0) sscanf(value,"%d",&screen_w);
	   if(strcmp(key,"HEIGHT")==0) sscanf(value,"%d",&screen_h);
	  // printf("sw:%d sh:%d\n\n\n",screen_w,screen_h);
 	
      
       fgets(buf,128,dispinfo);
       *(delim=strchr(buf,':'))='\0';
	   sscanf(buf,"%s",key);
	   sscanf(delim+1,"%s",value);
	   if(strcmp(key,"WIDTH")==0) sscanf(value,"%d",&screen_w);
	   if(strcmp(key,"HEIGHT")==0) sscanf(value,"%d",&screen_h);
	  // printf("sw:%d sh:%d\n\n\n",screen_w,screen_h);
	  
       
	fclose(dispinfo);
	assert(screen_w>0&&screen_h>0);

}


//-------------------------------------------------------------------------------------


void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  printf("1.5?\n");
  evtdev=fopen("/dev/events","r");
  fbdev=fopen("/dev/fb","w");
  printf("1.8\n");
  return 0;
}

void NDL_Quit() {
	now.tv_sec=now.tv_usec=0;
	if(canvas) free(canvas);

	fclose(evtdev);
	fclose(fbdev);
}
