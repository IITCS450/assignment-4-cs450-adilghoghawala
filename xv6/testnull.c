#include "types.h"
#include "stat.h"
#include "user.h"

int main(void){
  printf(1, "testnull: about to dereference NULL\n");

  volatile uint addr = 0;
  volatile int x = *(volatile int*)addr;

  printf(1, "UNEXPECTED: read=%d\n", x);
  exit();
}
