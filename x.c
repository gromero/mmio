#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

int main(void) {
  int s;
  int fd = open("/dev/stdout", O_RDWR);
  const char *text = "hello";
  size_t textsize = strlen(text) + 1;
  // For some unknown reason if a write() is not performed
  // before memcpy, writing to *map is invalid, generating
  // a SIGBUS. 
  s = lseek(fd, textsize, SEEK_SET);
  write(fd, "", 1);
  char *map = mmap(0, textsize, PROT_WRITE, MAP_SHARED, fd, 0);
  memcpy(map, text, strlen(text)+1);
  msync(map, textsize, MS_SYNC);
  munmap(map, textsize);
  close(fd);
  return 0;
}
