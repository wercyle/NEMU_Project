#include <klib.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

//size_t   unsigned int
size_t strlen(const char *s) {
  const char *ptr;
  for(ptr=s;*ptr!=0;++ptr);
  return ptr-s;
}

size_t strnlen(const char *s,size_t n){
   const char *ptr;
   for(ptr=s;(*ptr!=0)&&(ptr-s<n);++ptr);
   return ptr-s;
}


char *strcpy(char* dst,const char* src) {
	char *r=dst;
	assert((dst!=NULL)&&(src!=NULL));
	while((*r++=*src++)!='\0');
	return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
    for(int i=0;i<n;++i){
        dst[i]=src[i];
	}
	return dst;
}

char* strcat(char* dst, const char* src) {
    char *tmp=dst;
	while(*tmp) tmp++;//after this while,tmp point to the \0 at the end of the dst string.
	while((*tmp++=*src++)!='\0'){assert((tmp!=NULL)&&(src!=NULL));}
	return dst;
}

int strcmp(const char* s1, const char* s2) {
    while(*s1==*s2){
		assert((s1!=NULL)&&(s2!=NULL));
		if(*s1=='\0') return 0;
		s1++;
		s2++;
	}
	return *s1-*s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    for(int i;i<n;++i){
       if(s1[i]!=s2[i]) return s1[i]-s2[i]; 
	}
	return 0;
}

void* memset(void* v,int c,size_t n) {
     const unsigned char uc=c;//unsigned char only takes 1byte,so means only takes last 8bit of c
	 unsigned char *su;
	 for(su=v;0<n;++su,--n){
		 *su=uc;
	 }
	 return v;
}

void* memmove(void* dst,const void* src,size_t n) {
     assert((dst!=NULL)&&(src!=NULL));
	 void *ret=dst;
	 if((dst<=src)||dst>src+n){
        while(n--){
        *(char*)dst=*(char*)src;
		 dst=(char*)dst+1;
		 src=(char*)src+1;
	 }
	 }
	 else{
        src=(char*)src+n-1;
		dst=(char*)dst+n-1;
		while(n--){
            *(char*)dst=*(char*)src;
			dst=(char*)dst-1;
			src=(char*)src-1;
		}
	 }
	 return ret;
}

void* memcpy(void* out, const void* in, size_t n) {
     assert((out!=NULL)&&(in!=NULL));
	 void *ret=out;
	 while(n--){
        *(char*)out=*(char*)in;
		 out=(char*)out+1;
		 in=(char*)in+1;
	 }
	 return ret;
}
/*
void *memcpy(void *out,const void* in,size_t n){
     unsigned char *src=(unsigned char*)in;
	 unsigned char *dst=(unsigned char *)out;
	 for(int i=0;i<n;++i){
        *(dst+i)=*(src+i);
	 }
	 return out;

}
*/
/*the difference between memcpy and strcpy
1 content:strcpy can only copy character string ,but memcpy can copy anything,include character array/int/struct/class
2 method:strcpy wont cease until reach \0,but memcpy need n to assign the copy length
3 use :  character sting:strcpy.  other datatype:memcpy.   
*/




int memcmp(const void* s1, const void* s2, size_t n) {
    if(!n) return 0;
	while((--n)&&(*(char*)s1==*(char*)s2)){
		s1=(char*)s1 +1;
		s2=(char*)s2 +1;
	}
	return (*(unsigned char*)s1-*(unsigned char*)s2);
}
/*the difference between memcmp ans strcmp
1 content:strcmp can only compare charater string,but memcmp can copy anthing,include character array/int/struct/class
2 method:strcmp wont cease until reach \0,but memcmp need to assign the compare length,
  which means if n > \0, then memcmp wont stop when meet the \0
3 use:   character string:strcmp.   other datatype:memcmp.
*/





#endif
