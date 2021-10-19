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

/**
 * Allocates array of bytes (memory pool) and initializes the memory allocator.
 * If some bytes have been used after calling ma_malloc(size), calling to ma_init() will result in clearing up the memory pool.
 */
void ma_init() {

    mem_chunk_header* ptr = (mem_chunk_header *) mem_pool;
    mem_chunk_header first;
    first.size = (byte)(MEM_POOL_SIZE - sizeof(mem_chunk_header)); 
    first.status = FREE;
    *ptr = first;   

}

/**
 * Requesting for the tsize bytes from memory pool.
 * If the request is possible, the pointer to the first possible address byte (right after its header) in memory pool is returned.
 */
void *ma_malloc(size tsize) {
    if(tsize >= MEM_POOL_SIZE-sizeof(mem_chunk_header)){
        return 0;
    }
    
    int finished = 1;
    byte *current = mem_pool;
    while(finished && current+tsize <= (mem_pool + MEM_POOL_SIZE)){
        //problem getting into if statements
        if(((mem_chunk_header*) current)->status == FREE && ((mem_chunk_header*) current)->size >= tsize){
            ((mem_chunk_header*) current)->status = ALLOCATED;
            ((mem_chunk_header*) current)->size = tsize;

            mem_chunk_header* ptr = (mem_chunk_header *) current;
            mem_chunk_header empty_header;
            empty_header.size = (byte)(mem_pool - current - sizeof(empty_header)); 
            empty_header.status = FREE;
            *ptr = empty_header;            

            byte *start = current + (byte)sizeof(empty_header);
            printf("debug first if");
            return start;
        }
        if(((mem_chunk_header*) mem_pool)->status == ALLOCATED){
            current = current + (byte)sizeof(mem_chunk_header) + (byte)((mem_chunk_header*) current)->size;
            printf("debug second if");
        }
        if (((mem_chunk_header*) mem_pool)->status == 0){
            printf("probem with header");
            return current; 
        }
        current += 1;
        
    }

    return 0;
}

/**
 * Releasing the bytes in memory pool which was hold by ptr, meaning makes those bytes available for other uses.
 * Implement also the coalescing behavior.
 */
void ma_free(void *ptr) {

    //TODO: add your code here

}

/**
 * This function is only for debugging. It prints out the entire memory pool.
 * Print info on headers and content of all elements in your pool
 */
void ma_print(void) {

    //TODO: add your code here

}
 
  
