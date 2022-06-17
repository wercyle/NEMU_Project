#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int tenten(int k){   //1-1,2-10,3-100,4-1000
	int ten=1;
	while(k>1){ten*=10,k--;}
	return ten;
}

void numputch(int n,int flag){ //flag is used when minus
	int d;   //substite
	if(flag==0) d=n;
	else  d=-n,putch(45);


	int k=1;   //
	while(d/10>=1) {k++,d/=10;}

	if(flag==0){
	for(int i=k;i>0;i--){
		int ten=tenten(i);
		putch((n/ten)%10+48);
	}
	}
	else if(flag==1){
	for(int i=k;i>0;i--){
		int ten=tenten(i);
		putch((-n/ten)%10+48);
	}

	}
}

static void print_Hex(unsigned int hex){
    int i=8;
	putch('0');
	putch('x');
	while(i--){
	   //putch('c');
       unsigned char c=(hex & 0xf0000000)>>28; //want the most high half byte(note:int have 4byte!)
	   putch(c < 0xa ? c+'0':c-0xa + 'a');
	   hex <<=4;
	   //putch('c');
	}
}


int printf(const char *fmt, ...) {
   va_list ap;
   va_start(ap,fmt);
   int i=0;
   
  
   while(fmt[i]!=0){
	   if(fmt[i]!='%'){
          putch(fmt[i]);
		  i++;
		  continue;
	   }
	   i++;

	   switch(fmt[i]){
          case 'd':
			  {
		    int d;
			d=va_arg(ap,int);
			if(d>=0){
		    numputch(d,0);
			}
			else if(d<0)
			{
            numputch(d,1);
			}
			break;
			  }
		  case 'p':
			  {
             int d;
             d=va_arg(ap,int);
			 print_Hex(d);
			 break;
			  }
		  case 's':
			  {
            char *ss;
			ss=va_arg(ap,char *);
			while(*ss!=0){putch(*ss);ss++;}
			break;
			  }
	   }
	   i++;
   }

   va_end(ap);

  return 0;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return 0;
}

int sprintf(char *out, const char *fmt, ...) {
	va_list ap;
	va_start(ap,fmt);
	char *stmp=out;
	int i=0;
	int rt=0;//return value

	while(fmt[i]!=0){
		if(fmt[i]!='%'){
           *stmp++=fmt[i];
		   i++;
		   rt++;
		   continue;
		}
		i++;//jmp % 

      switch( fmt[i]){
		case 'd':{
		  int d;
		  d=va_arg(ap,int);//we get int,put in char, so we have to trans to ascii
		  if((d>=0)&&(d<=9)){
			  *stmp++=d+48;
			  rt++;
		  }
		  else if(d<=99){
			  *stmp++=d/10+48;
			  *stmp++=d%10+48;
			  rt+=2;
		  }
		  else if(d<=999){
              *stmp++=d/100+48;
			  *stmp++=(d/10)%10+48;
			  *stmp++=d%10+48;
			  rt+=3;
		  }
		  break;
		 }
	    case 's':{
		  char *ss;
		  ss=va_arg(ap,char*);
		  while((*stmp++=*ss++)!=0){rt++;};
	      break;
		}
	  }
	  i++;
	}
	*stmp='\0';
	va_end(ap);

  return rt;
}

void int2string(char* s,int d){
     int i=0;
	 while((d=d/10)!=0) i++;

}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  return 0;
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  return 0;
}

#endif
