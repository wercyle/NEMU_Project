#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
//  kbd->keydown = 0;
//  kbd->keycode = AM_KEY_NONE;

//  if((inb(0x64))&0x1) kbd->keycode=inl(KBD_ADDR),kbd->keydown=1;
//  else kbd->keycode=AM_KEY_NONE,kbd->keydown=0;

//	uint32_t kbd->keycode=AM_KEY_NONE;
//	if(inl(KBD_ADDR)&KEYDOWN_MASK){ kbd->keycode=inl(KBD_ADDR)&~KEYDOWN_MASK;kbd->keydown=true;}
//	else {kbd->keycode=inl(KBD_ADDR)&~KEYDOWN_MASK;kbd->keydown=false;}

	uint32_t num=inl(KBD_ADDR);

	if((num&KEYDOWN_MASK)==KEYDOWN_MASK) {kbd->keydown=true;}
	else kbd->keydown=false;
	
	kbd->keycode=num&~KEYDOWN_MASK;
		
}
