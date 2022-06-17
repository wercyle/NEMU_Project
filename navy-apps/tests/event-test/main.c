#include <stdio.h>
#include <NDL.h>


/*
int main() {
  NDL_Init(0);
  printf("come in event test\n");
  while (1) {
    char buf[64];
	int a=NDL_PollEvent(buf,sizeof(buf));

    if (a==1) {
	  printf("receive event: %s \n", buf);
    }
//	buf[0]='t';
//	printf("return:%d\n",NDL_PollEvent(buf,sizeof(buf)));

  }

  NDL_Quit();
  return 0;
}



*/
/*
int main(){
   FILE *fp=fopen("/dev/events","r");
   volatile int j=0;
   while(1){
      j++;
	  if(j==1000){
		 // printf("come in\n");
         char buf[256];
		 char ch;
		 char *p=buf;
		 while((ch=fgetc(fp))!=-1){
              *p++=ch;
			  if(ch=='\n'){*p='\0';break;}
		 }

	//	 p=buf;
	     if(buf[0]=='k') printf("receive event:%s\n",buf);
		 j=0;
 
	 }

   }

   fclose(fp);
   return 0;

}
*/
int main(){
   NDL_Init(0);
   volatile int j=0;

   while(1){
      j++;
	  if(j==1000){
		  char buf[64];
		 NDL_PollEvent(buf,sizeof(buf));

	//	 p=buf;
	     if(buf[0]=='k') printf("receive event:%s\n",buf);
		 j=0;
 
	 }

   }

   NDL_Quit();
   return 0;

}
