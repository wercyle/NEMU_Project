#include <stdio.h>
#include <assert.h>

int main() {

  FILE *fp = fopen("/share/files/num", "r+");
  if(fp==NULL)printf("fp==NULL\n"); 
 assert(fp);

  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);
  assert(size == 5000);

  fseek(fp, 500 * 5, SEEK_SET);
  int i, n;
  for (i = 500; i < 1000; i ++) {
    fscanf(fp, "%d", &n);
    assert(n == i + 1);
  }
//write(1,"PASS\n",5);
  fseek(fp, 0, SEEK_SET);
  for (i = 0; i < 500; i ++) {
    fprintf(fp, "%4d\n", i + 1 + 1000);
  }

  for (i = 500; i < 1000; i ++) {
    fscanf(fp, "%d", &n);
    assert(n == i + 1);
  }

  fseek(fp, 0, SEEK_SET);
  for (i = 0; i < 500; i ++) {
    fscanf(fp, "%d", &n);
    assert(n == i + 1 + 1000);
  }

  fclose(fp);

 // write(1,"PASS\n",5);
 printf("PASS!!!\n");
// int b=_syscall_(0,0,0,0);

  return 0;
}
