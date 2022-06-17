#include <stdio.h>
#include <sys/time.h>
#include <NDL.h>
#include <stdlib.h>


int main(){
	/* before encapsulation
     struct timeval * tv;
	int sec=1;
	while(1){
		 gettimeofday(tv,NULL);
         while((tv->tv_usec/1000000)<sec) gettimeofday(tv,NULL);
		 if(sec==1){
            printf("%d second).\n",sec);
		 }
		 else {
            printf("%d seconds).\n",sec);
		 }
		 sec++;
	}
	*/

	int sec=1;
	while(1){
        while(NDL_GetTicks()/1000000<sec);

		if(sec==1){
           printf("%d seconds).\n",sec);
		}
		else{
           printf("%d seconds).\n",sec);
		}
		sec++;
	}

	

}
