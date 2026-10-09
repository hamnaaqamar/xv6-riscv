#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define SEPARATORS " -\r\t\n./,"

int is_separator(char c) {
  return strchr(SEPARATORS, c) != 0;
}

void process_fd(int fd) {
  char c;
  int current_num = 0;
  int in_number = 0;
  int is_valid_digit_seq = 1;

  while (read(fd, &c, 1) > 0) {
    if (is_separator(c)) {
      if (in_number) {
        if (is_valid_digit_seq && (current_num % 5 == 0 || current_num % 6 == 0)) {
          printf("%d\n", current_num);
        }
        in_number = 0;
        current_num = 0;
        is_valid_digit_seq = 1;
      }
    } else if (c >= '0' && c <= '9') {
      if (!in_number) {
        in_number = 1;
        current_num = c - '0';
        is_valid_digit_seq = 1;
      } else if (is_valid_digit_seq) {
        current_num = current_num * 10 + (c - '0');
      }
    } else {
      // Non-digit non-separator (e.g. 'x' or 'v' in "xv6")
      if (in_number) {
        is_valid_digit_seq = 0;
      } else {
        // If we hit letters before numbers, invalidate the upcoming sequence
        in_number = 1;
        is_valid_digit_seq = 0;
      }
    }
  }

  // Handle EOF
  if (in_number && is_valid_digit_seq) {
    if (current_num % 5 == 0 || current_num % 6 == 0) {
      printf("%d\n", current_num);
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    process_fd(0);
  } else {
    for (int i = 1; i < argc; i++) {
      int fd = open(argv[i], 0);
      if (fd < 0) {
        fprintf(2, "sixfive: cannot open %s\n", argv[i]);
        continue;
      }
      process_fd(fd);
      close(fd);
    }
  }
  exit(0);
}