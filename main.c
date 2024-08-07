#include <stdio.h>
#include <unistd.h>
#include "malloc.h"

int main(int argc, char *argv[])
{
  void * initial_break = sbrk(0);

  void *val = malloc(320000);
  void *new_break = sbrk(0);
  size_t allocated_size = (char *) new_break - (char*) initial_break;
  printf("%zu\n", allocated_size);
  printf("%d\n",(int) getTail()->m.size);
  
  void *new = realloc(val, 480000);
  new_break = sbrk(0);
  allocated_size = (char *) new_break - (char*) initial_break;
  
  printf("%zu\n", allocated_size);
  printf("%d\n",(int) getTail()->m.size);

  return 0;
}
