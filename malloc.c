#include "malloc.h"
#include "string.h"

#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

mem_header_t *head, *tail; // Start and end of linked list

pthread_mutex_t global_mem_block_lock;

/*
 brk() returns zero. On error, -1 and errno is net to ENOMEM
 sbrk() returns the previous program break. (if the break was 
 increased, then this value is a pointer to the start of the newly allocated memory)
 On error, (void *) -1 is returned and errno is to ENOMEM
 */
void *malloc(size_t size){
  if (!size)
    return NULL;

  void *block;
  size_t total_block_size;
  mem_header_t *mem_header;
  pthread_mutex_lock(&global_mem_block_lock);
  // See if there is a free block that already satifies the required memory
  mem_header = get_bte_freed_memory_block(size);
  if(mem_header){
    mem_header->m.is_free = 0;
    pthread_mutex_unlock(&global_mem_block_lock);
    return (void *) mem_header + 1; // Give block that is freed back (memory after header `header + 1 byte`)
  }
  // Calculate the total size of memory alllocated
  total_block_size = sizeof(mem_header_t) + size;
  // Assign the memory returned from sbrk ( system call to increment the break pointer for program) to block
  block = sbrk(total_block_size);
  if ((void *) -1 == block){
    pthread_mutex_unlock(&global_mem_block_lock);
    return NULL;
  }
  // Assign the block to the header
  mem_header = block;
  // Set the size of the m size (memory block size) = to the requested size
  mem_header->m.size = size;
  // Set the memory to used
  mem_header->m.is_free = 0;
  // If new block is created by sbrk then its the last element
  // so set the next pointer to NULL
  mem_header->m.next = NULL;
  // Check if the head pointer is NULL if not assign to current
  if(!head)
    head = mem_header;
  // If tail exists set tail to the current header
  if(tail)
    tail->m.next = mem_header;
  // Set tail to mem_header, this should always happen as new block should be equal to the last element
  tail = mem_header;
  
  // Unlock the mutex lock
  pthread_mutex_unlock(&global_mem_block_lock);

  // Return the actual block that was asked for
  return (void *)(mem_header + 1);
}

// Get a current piece of memory that is bigger or equal
// to requested size this memory block has to bee freed
mem_header_t *get_bte_freed_memory_block(size_t size){
  mem_header_t *current_block = head;
  while(current_block){
    if (current_block->m.is_free && current_block->m.size >= size)
      return current_block;
    current_block = current_block->m.next;
  }
  return NULL;
}

// Allocate memory for a array and set initial data to 0
void *calloc(size_t number_elements, size_t element_size) {
  // Check if parameters are correct
  if (!number_elements || !element_size)
    return NULL;

  // Total block of memory;
  void *block;
  // Size calcualte variable
  size_t size;
  // Calculate the size
  size = number_elements * element_size;
  // It's possible for integers to overflow with multiplication like this.
  // Overflow is checked by comparing the size of a element in the array
  // to the total size devided by the number of elements
  if (element_size != size / number_elements)
    return NULL;
  
  // Malloc the block
  block = malloc(size);
  // Check if malloc was successful
  if (!block)
    return NULL;
  // Set the block of memory to 0 and for the total size
  memset(block, 0, size);
  return  block;
}


// This function is meant to be used as a reallocation of memory it will grow block if needed
void *realloc(void *memory_block, size_t size){
  // if memory_block is NULL or size is 0 return allocated memory of size(size) (this reallocates memory)
  // this means if size > 0, allocated memory with size(size) will be returned (this reallocates memory to size 0)
  if (!memory_block || !size)
    return malloc(size);

  // Memory Header
  // Get the header of the memory block
  mem_header_t *mem_header = (mem_header_t*)memory_block - 1;
  // if the size of the block is already bigger or the 
  // same size as the requested size then requirements 
  // are already met
  if (mem_header->m.size >= size)
    return memory_block;

  // The new memory block with the requested size
  void *new_block = malloc(size);
  // Check if malloc was successful
  if (new_block) {
    // Copy the memory from the old small memory_block
    // to the new bigger memory_block
    memcpy(new_block, memory_block, mem_header->m.size);
    // Free the old block
    free(memory_block);
  }
  
  // Return the new bigger memory_block
  // with old memory_block content
  return new_block;

}

void free(void *block) {
  // If block is NULL just return
  if(!block)
    return;

  mem_header_t *mem_header, *temp;
  void *program_break;
  
  // Lock the global mutex
  pthread_mutex_lock(&global_mem_block_lock);
  // Get the header of the current block by decrementing block by 1
  mem_header = (mem_header_t *) block -1;
  
  // Get the current probram break
  program_break = sbrk(0);
  
  // if the block being freed is at the end of the heap the memory 
  // will not be freed, but will be released back to OS
  if((char*)block + mem_header->m.size == program_break){
    // Last entry ot the W-linked list
    if(head == tail)
      head = tail = NULL;
    else{
      // Assing head to the temp var, so we can loop through the heap
      temp = head;
      // loop through the heap
      while(temp) {
        // temp is the second to last entry
        if(temp->m.next == tail) {
          // set the second to last entry's next pointer to NULL and tail to the second to last
          temp->m.next = NULL;
          tail = temp;
        }
        // Next block in the address space
        temp = temp->m.next;
      }
    }
    // Remove the memory from the heap and release back to the OS.
    sbrk(0 - sizeof(mem_header_t) - head->m.size);
    // Unlock the mutex
    pthread_mutex_unlock(&global_mem_block_lock);
    return;
  }
  // Set the block to free so it can be reallocated
  mem_header->m.is_free = 1;
  // Unlock the global mutex
  pthread_mutex_unlock(&global_mem_block_lock);
}

mem_header_t * getTail(){
  if(tail)
    return tail;
  return NULL;
}
