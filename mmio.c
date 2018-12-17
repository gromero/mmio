#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

/*
 Based upon James Fisher code found at:
 https://jameshfisher.com/2017/01/28/mmap-file-write.html
*/

/****
-> fd = open("./fakeout")

$ echo "abcdefghjklmn" > fakeout
$ ./x
$ # OK
$ >fakeout
 ./x
Bus error (core dumped)
$ # FAILED

-> fd = open("/dev/stdout")

$ vim x.c
$ make x
cc     x.c   -o x
$ ./x
Segmentation fault (core dumped)
$ # FAILED
$ ./x 1>>./fakeout
Bus error (core dumped)
$ # FAILED
$ echo "abcdefghjklmn" > fakeout
$ ./x 1>>./fakeout
$ # OK

****/

int main(void) {
  int s;
  // You need O_RDWR for MAP_SHARED accordingly to mmap()'s man.
  // See section about EACCES.
//int fd = open("/dev/stdout", O_RDWR);
  int fd = open("./fakeout", O_RDWR);
  const char *text = "hello";
  size_t textsize = strlen(text) + 1;
  // You will need ftruncate or lseek+write below if it's
  // a newly created file or if fd is opened against /dev/stdout,
  // even if it's redirected with 1>./fakeout, because fd will
  // have zero size. Otherwise, i.e. a non-empty file is opened,
  // mmap() can be used directly to write on the fd. If fd is
  // not truncated (grown), any write to *map will cause a SIGBUS crash.
  // On the other hand, If 1>>./fakeout is used, i.e. >> instead of >,
  // AND file size is >= 6 bytes, then no truncation (grow) is needed.
//s = lseek(fd, textsize, SEEK_SET);
//write(fd, "", 1);
  char *map = mmap(0, textsize, PROT_WRITE, MAP_SHARED, fd, 0);
  memcpy(map, text, strlen(text)+1);
  msync(map, textsize, MS_SYNC);
  munmap(map, textsize);
  close(fd);
  return 0;
}
