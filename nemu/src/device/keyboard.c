#include <device/map.h>
#include <monitor/monitor.h>
#include <SDL2/SDL.h>

#define I8042_DATA_PORT 0x60
#define I8042_DATA_MMIO 0xa1000060

static uint32_t *i8042_data_port_base = NULL;   //32wei ptr





//macro 1
// Note that this is not the standard
#define _KEYS(f) \
  f(ESCAPE) f(F1) f(F2) f(F3) f(F4) f(F5) f(F6) f(F7) f(F8) f(F9) f(F10) f(F11) f(F12) \
f(GRAVE) f(1) f(2) f(3) f(4) f(5) f(6) f(7) f(8) f(9) f(0) f(MINUS) f(EQUALS) f(BACKSPACE) \
f(TAB) f(Q) f(W) f(E) f(R) f(T) f(Y) f(U) f(I) f(O) f(P) f(LEFTBRACKET) f(RIGHTBRACKET) f(BACKSLASH) \
f(CAPSLOCK) f(A) f(S) f(D) f(F) f(G) f(H) f(J) f(K) f(L) f(SEMICOLON) f(APOSTROPHE) f(RETURN) \
f(LSHIFT) f(Z) f(X) f(C) f(V) f(B) f(N) f(M) f(COMMA) f(PERIOD) f(SLASH) f(RSHIFT) \
f(LCTRL) f(APPLICATION) f(LALT) f(SPACE) f(RALT) f(RCTRL) \
f(UP) f(DOWN) f(LEFT) f(RIGHT) f(INSERT) f(DELETE) f(HOME) f(END) f(PAGEUP) f(PAGEDOWN)

//macro 2
#define _KEY_NAME(k) _KEY_##k,


enum {
  _KEY_NONE = 0,
  MAP(_KEYS, _KEY_NAME)  //_KEYS(_KEY_NAME)----macro1----{ _KEY_NAME(ESCAPE), _KEY_NAME(F1),...}---macro2----enum{_KEY_NONE _KEY_ESCAPE,_KEY_F1,...};
};


//macro3
#define SDL_KEYMAP(k) [concat(SDL_SCANCODE_, k)] = concat(_KEY_, k),

static uint32_t keymap[256] = { 
  MAP(_KEYS, SDL_KEYMAP)  //_KEYS(SDL_KEY_KEYMAP)-----macor1-----{SDL_KEYMAP(ESCAPE),SDL_KEYMAP(F1),...}----macro3-----keymap[256]={[SDL_SCANCODE_ESCAPE]=_KEY_ESCAPE,...};
};






#define KEY_QUEUE_LEN 1024
static int key_queue[KEY_QUEUE_LEN] = {};
static int key_f = 0, key_r = 0;

#define KEYDOWN_MASK 0x8000  //10000000 00000000

void send_key(uint8_t scancode, bool is_keydown) { //used in where?   in device.c(device_update())
  if (nemu_state.state == NEMU_RUNNING &&
      keymap[scancode] != _KEY_NONE) {

    uint32_t am_scancode = keymap[scancode] | (is_keydown ? KEYDOWN_MASK : 0);//1 reality key  to  virtual key,
	                                                                          //2 if event is keydown or keyup,all will call send_key() in device_update(),
																            	  //and how to distinguish them,use KEYDOWN_MASK.
    key_queue[key_r] = am_scancode;
    key_r = (key_r + 1) % KEY_QUEUE_LEN;

    Assert(key_r != key_f, "key queue overflow!");
  }
}

static void i8042_data_io_handler(uint32_t offset, int len, bool is_write) {
  assert(!is_write);
  assert(offset == 0);
  if (key_f != key_r) {
    i8042_data_port_base[0] = key_queue[key_f];
    key_f = (key_f + 1) % KEY_QUEUE_LEN;
  }
  else {
    i8042_data_port_base[0] = _KEY_NONE;
  }
}

void init_i8042() {
  i8042_data_port_base = (void *)new_space(4);
  i8042_data_port_base[0] = _KEY_NONE;
  add_pio_map("keyboard", I8042_DATA_PORT, (void *)i8042_data_port_base, 4, i8042_data_io_handler);
  add_mmio_map("keyboard", I8042_DATA_MMIO, (void *)i8042_data_port_base, 4, i8042_data_io_handler);
}
