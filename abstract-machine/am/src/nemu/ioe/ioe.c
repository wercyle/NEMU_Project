#include <am.h>
#include <klib-macros.h>
//---------------------------total function:18, amdev-uart-net(only net config) ---------------------
void __am_timer_init();
void __am_gpu_init();
void __am_audio_init();

void __am_input_keybrd(AM_INPUT_KEYBRD_T *);
void __am_timer_rtc(AM_TIMER_RTC_T *);
void __am_timer_uptime(AM_TIMER_UPTIME_T *);//realize in time.c
void __am_gpu_config(AM_GPU_CONFIG_T *);
void __am_gpu_status(AM_GPU_STATUS_T *);
void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *);
void __am_audio_config(AM_AUDIO_CONFIG_T *);
void __am_audio_ctrl(AM_AUDIO_CTRL_T *);
void __am_audio_status(AM_AUDIO_STATUS_T *);
void __am_audio_play(AM_AUDIO_PLAY_T *);

static void __am_timer_config(AM_TIMER_CONFIG_T *cfg) { cfg->present = true; cfg->has_rtc = true; }
static void __am_input_config(AM_INPUT_CONFIG_T *cfg) { cfg->present = true;  }
static void __am_uart_config(AM_UART_CONFIG_T *cfg)   { cfg->present = false; }
static void __am_disk_config(AM_DISK_CONFIG_T *cfg)   { cfg->present = false; }
static void __am_net_config (AM_NET_CONFIG_T *cfg)    { cfg->present = false; }
//----------------------------------------------------------------------------------------------------


//--------------------------total function:15, above -init()-----------------------------------------
typedef void (*handler_t)(void *buf);

static void *lut[128] = {
  [AM_TIMER_CONFIG] = __am_timer_config,//4
  [AM_TIMER_RTC   ] = __am_timer_rtc,   //5
  [AM_TIMER_UPTIME] = __am_timer_uptime,//6

  [AM_INPUT_CONFIG] = __am_input_config,//7
  [AM_INPUT_KEYBRD] = __am_input_keybrd,//8

  [AM_GPU_CONFIG  ] = __am_gpu_config,//9
  [AM_GPU_FBDRAW  ] = __am_gpu_fbdraw,//11
  [AM_GPU_STATUS  ] = __am_gpu_status,//10

  [AM_UART_CONFIG ] = __am_uart_config,//1
  [AM_AUDIO_CONFIG] = __am_audio_config,//14
  [AM_AUDIO_CTRL  ] = __am_audio_ctrl,//15
  [AM_AUDIO_STATUS] = __am_audio_status,//16
  [AM_AUDIO_PLAY  ] = __am_audio_play,//17
  [AM_DISK_CONFIG ] = __am_disk_config,//18
  [AM_NET_CONFIG  ] = __am_net_config,//21
};
//---------------------------------------------------------------------------------------------------

static void fail(void *buf) { panic("access nonexist register"); }

bool ioe_init() {
  for (int i = 0; i < LENGTH(lut); i++)
    if (!lut[i]) lut[i] = fail;
  __am_gpu_init();
  __am_timer_init();
  __am_audio_init();
  return true;
}

void ioe_read (int reg, void *buf) { ((handler_t)lut[reg])(buf); }
void ioe_write(int reg, void *buf) { ((handler_t)lut[reg])(buf); }


