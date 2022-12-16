#include <stdio.h>

struct String {
  size_t len;
  
};

void nprintf(const char *fmt, ...) {
  va_list argp;
  va_start(argp, fmt);
  while 

  fputs("Test", stdout);
}

int main() {
  nprintf("Test %d\n", 30);
  return 0;
}
