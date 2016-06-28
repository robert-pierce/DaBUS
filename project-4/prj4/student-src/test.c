#include<stdio.h>
#include "page-splitting.h"

int page_size = 256;

main() {
  int address = 0xB42;
  int vpn = VADDR_PAGENUM(address);
  int offset = VADDR_OFFSET(address);

  printf("The VPN is: %d\n", vpn);
  printf("The offset is: %d\n", offset);
  return 0;
}
