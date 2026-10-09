#include "kernel/types.h"
#include "user/user.h"

void memdump(char *fmt, char *data, int len) {
  int offset = 0;

  for (int i = 0; fmt[i] != '\0'; i++) {
    char c = fmt[i];

    switch (c) {
    case 'i': {
      if (offset + 4 > len) {
        printf("memdump: not enough data for 'i'\n");
        return;
      }
      int val = *(int *)(data + offset);
      printf("%d\n", val);
      offset += 4;
      break;
    }
    case 'p': {
      if (offset + 8 > len) {
        printf("memdump: not enough data for 'p'\n");
        return;
      }
      uint64 val = *(uint64 *)(data + offset);
      printf("%p\n", (void *)val);
      offset += 8;
      break;
    }
    case 'h': {
      if (offset + 2 > len) {
        printf("memdump: not enough data for 'h'\n");
        return;
      }
      short val = *(short *)(data + offset);
      printf("%d\n", val);
      offset += 2;
      break;
    }
    case 'c': {
      if (offset + 1 > len) {
        printf("memdump: not enough data for 'c'\n");
        return;
      }
      char val = *(data + offset);
      printf("%c\n", val);
      offset += 1;
      break;
    }
    case 's': {
      if (offset + 8 > len) {
        printf("memdump: not enough data for 's'\n");
        return;
      }
      char *str_ptr = *(char **)(data + offset);
      printf("%s\n", str_ptr);
      offset += 8;
      break;
    }
    case 'S': {
      while (offset < len && data[offset] != '\0') {
        printf("%c", data[offset]);
        offset++;
      }
      printf("\n");
      if (offset < len && data[offset] == '\0') {
        offset++;
      }
      break;
    }
    default:
      break;
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc > 1) {
    char buf[512];
    int n, len = 0;
    while ((n = read(0, buf + len, sizeof(buf) - len)) > 0) {
      len += n;
    }
    memdump(argv[1], buf, len);
  } else {
    // Default test examples from assignment specification
    int a1[] = {61810, 2026};
    printf("Example 1:\n");
    memdump("ii", (char *)a1, sizeof(a1));

    char *a2 = "a string";
    printf("Example 2:\n");
    memdump("s", (char *)&a2, sizeof(a2));

    struct {
      char *s;
    } a3 = {"another"};
    printf("Example 3:\n");
    memdump("s", (char *)&a3, sizeof(a3));

    struct {
      uint64 p;
      int i;
      short h;
      char c;
      char s[8];
    } a4 = {0xBD0, 1819438967, 100, 'z', "xyzzy"};
    printf("Example 4:\n");
    memdump("pihcS", (char *)&a4, sizeof(a4));

    char a5[] = "hello\0w\0o\0r\0l\0d\0";
    printf("Example 5:\n");
    memdump("SSSSSS", a5, sizeof(a5));
  }
  exit(0);
}