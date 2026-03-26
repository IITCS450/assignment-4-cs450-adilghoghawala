#include "types.h"
#include "stat.h"
#include "user.h"

int main(void) {
  // Allocate a page
  char *addr = sbrk(4096);
  
  addr[0] = 'A';
  printf(1, "testmprotect: write before protect OK\n");

  // Protect the page
  if(mprotect(addr, 1) < 0){
    printf(1, "testmprotect: mprotect failed\n");
    exit();
  }
  printf(1, "testmprotect: mprotect OK\n");

  printf(1, "testmprotect: attempting write to protected page...\n");
  addr[0] = 'B';

  printf(1, "UNEXPECTED: write succeeded\n");
  exit();
}
