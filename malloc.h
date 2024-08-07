#include "universal.h"
#include <stdio.h>

void *malloc(size_t size);

mem_header_t *get_bte_freed_memory_block(size_t);

mem_header_t *getTail();

void *calloc(size_t number_elements, size_t element_size);

void *realloc(void *memory_block, size_t size);

void free(void *block);


