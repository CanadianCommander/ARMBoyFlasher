#include <stdio.h>

int data=0;
void moduleInit(void * arg){
  printf("is it.... working? %d\n",data);
  data++;
}
