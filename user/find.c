#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

char* fmtname(char *path) {
  static char buf[DIRSIZ+1];
  char *p;

  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p) + 1);
  return buf;
}

void run_exec(char *path, int argc, char *argv[]) {
  int pid = fork();
  if(pid < 0) {
    fprintf(2, "find: fork failed\n");
    return;
  }
  if(pid == 0) {
    // Child process
    // Construct new argument list appending path
    char *new_argv[MAXARG];
    int i;
    for(i = 0; i < argc && i < MAXARG - 2; i++) {
      new_argv[i] = argv[i];
    }
    new_argv[i++] = path;
    new_argv[i] = 0;

    exec(new_argv[0], new_argv);
    fprintf(2, "find: exec %s failed\n", new_argv[0]);
    exit(1);
  } else {
    // Parent process waits for child
    wait(0);
  }
}

void find(char *path, char *filename, int exec_mode, int exec_argc, char *exec_argv[]) {
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(path, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    if(strcmp(fmtname(path), filename) == 0){
      if(exec_mode){
        run_exec(path, exec_argc, exec_argv);
      } else {
        printf("%s\n", path);
      }
    }
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf("find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      // Skip "." and ".."
      if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", buf);
        continue;
      }
      
      if(strcmp(fmtname(buf), filename) == 0){
        if(exec_mode){
          run_exec(buf, exec_argc, exec_argv);
        } else {
          printf("%s\n", buf);
        }
      }
      
      if(st.type == T_DIR) {
        find(buf, filename, exec_mode, exec_argc, exec_argv);
      }
    }
    break;
  }
  close(fd);
}

int main(int argc, char *argv[]) {
  if(argc < 3){
    fprintf(2, "Usage: find <path> <filename> [-exec cmd ...]\n");
    exit(1);
  }

  int exec_mode = 0;
  int exec_argc = 0;
  char **exec_argv = 0;

  for(int i = 3; i < argc; i++){
    if(strcmp(argv[i], "-exec") == 0){
      exec_mode = 1;
      exec_argc = argc - (i + 1);
      exec_argv = &argv[i + 1];
      break;
    }
  }

  if(exec_mode && exec_argc == 0){
    fprintf(2, "find: -exec requires a command\n");
    exit(1);
  }

  find(argv[1], argv[2], exec_mode, exec_argc, exec_argv);
  exit(0);
}