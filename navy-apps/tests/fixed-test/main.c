#include <fixedptc.h>
#include <stdio.h>


int main(){
      fixedpt a=fixedpt_rconst(1.2);
	  fixedpt b=fixedpt_fromint(10);
	  int c=0;
	  
	  if(b>fixedpt_rconst(7.9)){

         //c=fixedpt_toint(fixedpt_rconst(-128));
         c=fixedpt_toint(fixedpt_rconst(-2000));
		// c|=0xffffff00;
	  }
       
	  printf("c:%+d\n",(uint32_t)c);



}


























