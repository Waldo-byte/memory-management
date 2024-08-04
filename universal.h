#include <stdio.h>

typedef char MEMALIGN[16] ; // Declaring as type so char does not have to be repeated

union memory_block_header{
  struct{
    size_t size; // Size of memory block
    unsigned is_free; // Is memory freed or not?
    union memory_block_header *next; // Pointer to the next block of memory
  } m ;
  MEMALIGN stub; // Memory alignment so memory header is always atleast 16 bytes
};

typedef union memory_block_header mem_header_t; // Declaring as type so union keyword does not have to be repeated
