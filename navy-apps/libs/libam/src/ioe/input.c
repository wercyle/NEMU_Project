#include <am.h>
#include <string.h>
#include <SDL.h>

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd){
		SDL_Event ev;
		ev.type=3;
		ev.key.keysym.sym=0;
		SDL_PollEvent(&ev);

		//kbd->keydown =(ev.type==SDL_KEYUP ? true :false);
		//kbd->keycode =ev.key.keysym.sym;
		if(ev.type==SDL_KEYUP){
			//	printf("1111\n");
				kbd->keydown=false;
				kbd->keycode=ev.key.keysym.sym;
		}
		else if(ev.type==SDL_KEYDOWN){
			//	printf("2222\n");
				kbd->keydown=true;
				kbd->keycode=ev.key.keysym.sym;
		}
		else if(ev.type==SDL_TIMER){
			//	printf("3333\n");
				kbd->keydown=false;
				kbd->keycode=AM_KEY_NONE;
		}

}
