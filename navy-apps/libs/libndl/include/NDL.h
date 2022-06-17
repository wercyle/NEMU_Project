#ifndef __NDL_H__
#define __NDL_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif


//macro1
#define _KEYS(_) \
  _(ESCAPE) _(F1) _(F2) _(F3) _(F4) _(F5) _(F6) _(F7) _(F8) _(F9) _(F10) _(F11) _(F12) \
  _(GRAVE) _(1) _(2) _(3) _(4) _(5) _(6) _(7) _(8) _(9) _(0) _(MINUS) _(EQUALS) _(BACKSPACE) \
  _(TAB) _(Q) _(W) _(E) _(R) _(T) _(Y) _(U) _(I) _(O) _(P) _(LEFTBRACKET) _(RIGHTBRACKET) _(BACKSLASH) \
  _(CAPSLOCK) _(A) _(S) _(D) _(F) _(G) _(H) _(J) _(K) _(L) _(SEMICOLON) _(APOSTROPHE) _(RETURN) \
  _(LSHIFT) _(Z) _(X) _(C) _(V) _(B) _(N) _(M) _(COMMA) _(PERIOD) _(SLASH) _(RSHIFT) \
  _(LCTRL) _(APPLICATION) _(LALT) _(SPACE) _(RALT) _(RCTRL) \
  _(UP) _(DOWN) _(LEFT) _(RIGHT) _(INSERT) _(DELETE) _(HOME) _(END) _(PAGEUP) _(PAGEDOWN)


//macro2
#define enumdef(k) NDL_SCANCODE_##k,

//macro1+macro2
enum NDL_ScanCode{
    NDL_SCANCODE_NONE=0,
	_KEYS(enumdef)
};



enum NDL_EventType{
    NDL_EVENT_KEYDOWN,
	NDL_EVENT_KEYUP,
	NDL_EVENT_TIMER,
};

typedef struct NDL_Event{
    enum NDL_EventType type;
	int32_t data;
}NDL_Event;










int NDL_Init(uint32_t flags);
void NDL_Quit();

uint32_t NDL_GetTicks();
//int NDL_PollEvent(char *buf, int len);
int NDL_PollEvent(NDL_Event *event);
int NDL_WaitEvent(NDL_Event *event);

void NDL_GetCanvasInfo(int *w,int *h);
void NDL_OpenCanvas(int *w, int *h);
void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h);
int NDL_Render();


void NDL_OpenAudio(int freq, int channels, int samples);
void NDL_CloseAudio();
int NDL_PlayAudio(void *buf, int len);
int NDL_QueryAudio();

#ifdef __cplusplus
}
#endif

#endif
