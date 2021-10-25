/**
 * \author Samuel Tocci
 */

#include "ma_malloc.h"
#include <stdio.h>

#define MEM_POOL_SIZE 600   //in bytes
typedef unsigned char byte;

//! An enum of statuses that can be used in the header
typedef enum {
    ALLOCATED, FREE
} mem_status;

//! Every item in the pool will have at least a header with size and status of the following data
typedef struct {
    size size;
    mem_status status;
} mem_chunk_header;


static byte mem_pool[MEM_POOL_SIZE];
const int header_size = (int) sizeof(mem_chunk_header);
/**
 * Allocates array of bytes (memory pool) and initializes the memory allocator.
 * If some bytes have been used after calling ma_malloc(size), calling to ma_init() will result in clearing up the memory pool.
 */
void ma_init() {
    for (int i = 0; i < MEM_POOL_SIZE;  i++){
        *(mem_pool +i) = 0x00;
    } 
    ((mem_chunk_header*) mem_pool)->size = MEM_POOL_SIZE - header_size;
    ((mem_chunk_header*) mem_pool)->status = FREE;
}

/**
 * Requesting for the tsize bytes from memory pool.
 * If the request is possible, the pointer to the first possible address byte (right after its header) in memory pool is returned.
 */
void *ma_malloc(size tsize) {
    void* ptr = NULL;
    
    int i = 0;
    mem_chunk_header* mem_chunk = (mem_chunk_header*) mem_pool;

    while (i<MEM_POOL_SIZE && (mem_chunk->status == ALLOCATED || mem_chunk->size < tsize)){
        mem_chunk = ((mem_chunk_header*)(mem_pool +i));
        if (mem_chunk->status == ALLOCATED || mem_chunk->size < tsize){
            int jump = (mem_chunk->size) + 2*header_size;
            i += jump;
        }   
    }

    if (i< MEM_POOL_SIZE){
        int freeSpace = mem_chunk->size - (tsize + 2*header_size);
        mem_chunk->size = tsize;
        mem_chunk->status = ALLOCATED;
        ptr = mem_chunk + 1;

        mem_chunk_header* footer = (mem_chunk_header*)(ptr +tsize);
        footer->size = tsize;
        footer->status = ALLOCATED;

        mem_chunk_header* new_header = (mem_chunk_header*)(ptr + tsize + header_size);
        new_header->size = freeSpace;
        new_header->status = FREE;
    }
    
    return ptr;
}

/**
 * Releasing the bytes in memory pool which was hold by ptr, meaning makes those bytes available for other uses.
 * Implement also the coalescing behavior.
 */
void ma_free(void *ptr) {
    size tsize = ((mem_chunk_header*)ptr)->size;
    
    for (int i = 0; i < tsize;  i++){
        *(ptr + i) = 0x00;
    }
      
    ((mem_chunk_header*)ptr)->status = FREE;
}

/**
 * This function is only for debugging. It prints out the entire memory pool.
 * Print info on headers and content of all elements in your pool
 */
void ma_print(void) {

    //TODO: add your code here

}
 
  
