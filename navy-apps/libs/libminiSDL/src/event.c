#include <NDL.h>
#include <SDL.h>
#include <assert.h>

//macro2
#define keyname(k) #k,
//macro1+macro2
static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

//#define numkeys (sizeof(keyname)/sizeof(keyname[0]))

/*
//macro2
#define sdlkdef(k) SDLK_##k,
//macro1+macro2
enum SDL_Keycode{
	SDLK_NONE=0,
	_KEYS(sdlkdef)
};
*/



int SDL_PushEvent(SDL_Event *ev) {
  assert(0);
  return 0;
}

int SDL_PollEvent(SDL_Event *event) {
  NDL_Event e;
  int rt=NDL_PollEvent(&e);
  if(e.type==NDL_EVENT_KEYDOWN){
	   event->type=SDL_KEYDOWN;
	   event->key.keysym.sym=e.data;
   }
   else if(e.type==NDL_EVENT_KEYUP){
       event->type=SDL_KEYUP;
	   event->key.keysym.sym=e.data;
   }
   else if(e.type==NDL_EVENT_TIMER){
       event->type=SDL_TIMER;
	   event->key.keysym.sym=0;
   }

   return rt;

}

int SDL_WaitEvent(SDL_Event *event) {
   NDL_Event e;
   int rt=NDL_WaitEvent(&e);
   if(e.type==NDL_EVENT_KEYDOWN){
	   event->type=SDL_KEYDOWN;
	   event->key.keysym.sym=e.data;
   }
   else if(e.type==NDL_EVENT_KEYUP){
       event->type=SDL_KEYUP;
	   event->key.keysym.sym=e.data;
   }
   else if(e.type==NDL_EVENT_TIMER){
       event->type=SDL_TIMER;
	   event->key.keysym.sym=0;
  }

   return rt;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  assert(0);
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  assert(0);
  return NULL;
}
