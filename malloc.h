#include "universal.h"
#include <stdio.h>

void *malloc(size_t size);
mem_header_t *get_bte_freed_memory_block(size_t);

void free(void *block);

