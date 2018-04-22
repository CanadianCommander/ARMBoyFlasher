#include <stdio.h>
int data ;
int dataMk2;

void foobar(){
  data = 0;
  dataMk2 = 42;
}

void moduleInit(void * arg){
  printf("foobar! %d %d\n", data, dataMk2);
  data ++;
  dataMk2++;
}
