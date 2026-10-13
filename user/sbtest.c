#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/syscall.h"

int
main(int argc, char *argv[])
{
  // Test program for sandboxing
  int mask = (1 << SYS_open);
  
  if(fork() == 0){
    interpose(mask, "-");
    int fd = open("README", 0);
    if(fd < 0){
      printf("sbtest: successfully blocked open()!\n");
      exit(0);
    } else {
      printf("sbtest: ERROR: open() was not blocked!\n");
      close(fd);
      exit(1);
    }
  }
  
  wait(0);
  exit(0);
}