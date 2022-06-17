#define SDL_malloc  malloc
#define SDL_free    free
#define SDL_realloc realloc

#define SDL_STBIMAGE_IMPLEMENTATION
#include "SDL_stbimage.h"

SDL_Surface* IMG_Load_RW(SDL_RWops *src, int freesrc) {
  assert(src->type == RW_TYPE_MEM);
  return NULL;
}

SDL_Surface* IMG_Load(const char *filename) {
    FILE *fp=fopen(filename,"r");
	assert(fp);

	fseek(fp,0,SEEK_END);
	int flen=ftell(fp);
	char *buf=(char*)SDL_malloc(flen);
	assert(buf);
	//printf("flen:%d\n",flen);
	fseek(fp,0,SEEK_SET);
	fread(buf,flen,1,fp);

	SDL_Surface* s=STBIMG_LoadFromMemory(buf,flen);
	assert(s);

	fclose(fp);
	SDL_free(buf);

	return s;
}

int IMG_isPNG(SDL_RWops *src) {
  return 0;
}

SDL_Surface* IMG_LoadJPG_RW(SDL_RWops *src) {
  return IMG_Load_RW(src, 0);
}

char *IMG_GetError() {
  return "Navy does not support IMG_GetError()";
}
