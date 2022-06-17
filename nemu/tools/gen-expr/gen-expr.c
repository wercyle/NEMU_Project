#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

#include "expr.h"

// this should be enough
static char buf[65536] = {};
static char code_buf[65536 + 128] = {}; // a little larger than `buf`
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";


int flag=0;
int choose(int n){
	int num=rand()%n;
	return num ;
}

void gen_num(){
	int num=rand()%9;
	buf[flag]=num+49;
	flag++;
}

void gen(char a){
	buf[flag]=a;
	flag++;
}

void gen_rand_op(){
	int num=rand()%40;
	if((num>=0)&&(num)<10) buf[flag]='+';
	else if((num>=10)&&(num<20)) buf[flag]='+';
	else if ((num>=20)&&(num<30)) buf[flag]='*';
	else if ((num>=30)&&(num<40)) buf[flag]='+';
	flag++;
}




static inline void gen_rand_expr() {
	switch (choose(3)){
		case 0:gen_num();break;
		case 1:gen('(');gen_rand_expr();gen(')');break;
		default: gen_rand_expr();gen_rand_op();gen_rand_expr();break;
	}
}

/*
static inline void gen_rand_expr(){
	strcpy(buf,"(1+2)*(3+4)");
}
*/

void init_buf(){
   for(int i=0;i<flag;i++){
	   buf[i]='\0';
   }
   flag=0;
}

int main(){
	int loop=30;
	int i;
	char s[1000];
    int d;
	char s1[1000];

	for(i=0;i<loop;i++){
		scanf("%s",s);
        sscanf(s,"%d%s",d,s1);
		int num=expr(s1);
		if(num==d) printf("Right\n");
		else printf("Wrong\n");
	}
}












/*
int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 3;
  
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  
  int i;
  for (i = 0; i < loop; i ++) {
    init_buf();
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    int a=fscanf(fp, "%d", &result);
	assert(a!=-1);
    pclose(fp);

    printf("%u %s\n ", result, buf);
  }

  return 0;
}
