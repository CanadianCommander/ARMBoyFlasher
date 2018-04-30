
/**
  example program that makes one call to the example module's moduleInit function.
*/
void main(void){
  for(;;){
    asm(
      "mov r0, 0x10000 @ module 0x1 jump vector 0x0\n"
      "svc 0x3 @ module system call\n"
      :
      :
      : "r0", "r1", "r2", "r3", "lr", "pc", "memory"
    );
  }
}
