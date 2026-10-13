#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int
main(int argc, char *argv[])
{
  int mask;

  if(argc < 3){
    fprintf(2, "Usage: sandbox mask path command...\n");
    exit(1);
  }

  mask = atoi(argv[1]);

  if(fork() == 0){
    if(interpose(mask, argv[2]) < 0){
      fprintf(2, "sandbox: interpose failed\n");
      exit(1);
    }
    exec(argv[3], &argv[3]);
    fprintf(2, "sandbox: exec %s failed\n", argv[3]);
    exit(1);
  }

  wait(0);
  exit(0);
}