#include <common.h>

#ifdef HAS_IOE

#define SHOW_SCREEN
//#define MODE_800x600

#ifdef MODE_800x600
# define SCREEN_W 800
# define SCREEN_H 600
#else
# define SCREEN_W 400
# define SCREEN_H 300
#endif
#define SCREEN_SIZE ((SCREEN_H * SCREEN_W) * sizeof(uint32_t))


#include <device/map.h>
#include <SDL2/SDL.h>


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//vga has two type of register:1 for vem 2 for SCREEN_H and SCREEN_W 3 for judge if need to refresh the screen
//but in init_vga,dont involve the register 3
//int the __am_gpu_fbdraw will determine if refresh(if(ctl->sync),outl the value to register 3),then in the update_screen function,wile check this register to decide whether refresh
#define VMEM 0xa0000000

#define VGACTL_PORT 0x100 // Note that this is not the standard
#define VGACTL_MMIO 0xa1000100

#define SYNC_ADDR (VGACTL_PORT+4)


static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;

static uint32_t (*vmem) [SCREEN_W] = NULL;
static uint32_t *vgactl_port_base = NULL;



static inline void update_screen() {
#ifdef SHOW_SCREEN
  SDL_UpdateTexture(texture, NULL, vmem, SCREEN_W * sizeof(vmem[0][0])); //vem > texture > render > present, but who write in vem?
  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);
#endif
}

void vga_update_screen() {
  // TODO: call `update_screen()` when the sync register is non-zero,
  // then zero out the sync register
  int flag=vgactl_port_base[1];
  if(flag!=0){
     update_screen();
	 vgactl_port_base[1]=0;
 }
}



void init_vga() {
#ifdef SHOW_SCREEN
  SDL_Window *window = NULL;
  char title[128];
  sprintf(title, "%s-NEMU", str(__ISA__));
  SDL_Init(SDL_INIT_VIDEO); //init the sub system:video system

//int SDLCALL SDL_Init(uint32_t flag) is used to init subsystem
//SDL_Window *SDLCALL SDL_Creat_CreateWindow(const char*title,int x,int y,int w,int h,uint32_t flags).   (struct SDL_Window is defined in video\SDL_sysvideo.h)
//SDL_Renderer *SDLCALL SDL_CreateRenderer(SDL_Window *window,int index,uint32_t flags).   (struct SDL_Renderer is defined in render\sysrender.h)
//SDL_Texture *SDLCALL SDL_CreateTexture(SDL_Renderer *renderer,uint32_t format,int access,int w,int h).   (struct SDL_Texture is defined in render\sysrender.h)
//int SDLCALL SDL_UpdateTexture(SDL_Texture *texture,const SDL_Rect *rect,const void *pixels,int pitch)

#ifdef MODE_800x600
  SDL_CreateWindowAndRenderer(SCREEN_W, SCREEN_H, 0, &window, &renderer);//SCEEEN_W is w,SCREEN_H is h,0 is flag,so where is x,y?
#else
  SDL_CreateWindowAndRenderer(SCREEN_W * 2, SCREEN_H * 2, 0, &window, &renderer);
#endif
  SDL_SetWindowTitle(window, title);
  texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,SDL_TEXTUREACCESS_STATIC, SCREEN_W, SCREEN_H);
#endif


  vgactl_port_base = (void *)new_space(8);
  vgactl_port_base[0] = ((SCREEN_W) << 16) | (SCREEN_H);
  add_pio_map("screen", VGACTL_PORT, (void *)vgactl_port_base, 8, NULL);
  add_mmio_map("screen", VGACTL_MMIO, (void *)vgactl_port_base, 8, NULL);

  vmem = (void *)new_space(SCREEN_SIZE);//vmem size=SCREEN_SIZE*uint32_t=300*400*uint32_t
  add_mmio_map("vmem", VMEM, (void *)vmem, SCREEN_SIZE, NULL);
}
#endif	/* HAS_IOE */
