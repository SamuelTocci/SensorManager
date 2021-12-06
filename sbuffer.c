/**
 * \author Samuel Tocci
 */
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include "sbuffer.h"
#include <pthread.h>
#include <semaphore.h>

/**
 * basic node for the buffer, these nodes are linked together to create the buffer
 */
typedef struct sbuffer_node {
    struct sbuffer_node *next;           /**< a pointer to the next node */
    sensor_data_t data;                  /**< a structure containing the data */
    sem_t reads_sem;                     /**< a semaphore to keep track of amount of reads done */
    //mss mutex toevoegen om reads_sem niet op hetzelfde moment te overwriten
} sbuffer_node_t;

/**
 * a structure to keep track of the buffer
 */
struct sbuffer {
    sbuffer_node_t *head;         /**< a pointer to the first node in the buffer */
    sbuffer_node_t *tail;         /**< a pointer to the last node in the buffer */
    pthread_rwlock_t head_rwlock; /**< a rwLock for the head of the sbuffer */
    pthread_rwlock_t tail_rwlock; /**< a rwLock for the tail of the sbuffer */
};

int sbuffer_init(sbuffer_t **buffer) { //thread safety not needed, only used when starting program
    *buffer = malloc(sizeof(sbuffer_t));
    if (*buffer == NULL) return SBUFFER_FAILURE;
    (*buffer)->head = NULL;
    (*buffer)->tail = NULL;
    return SBUFFER_SUCCESS;
}

int sbuffer_free(sbuffer_t **buffer) { //thread safety not needed, only used when shutting down program
    sbuffer_node_t *dummy;
    if ((buffer == NULL) || (*buffer == NULL)) {
        return SBUFFER_FAILURE;
    }
    while ((*buffer)->head) {
        dummy = (*buffer)->head;
        (*buffer)->head = (*buffer)->head->next;
        free(dummy);
    }
    free(*buffer);
    *buffer = NULL;
    return SBUFFER_SUCCESS;
}

int sbuffer_remove(sbuffer_t *buffer, sensor_data_t *data) {
    sbuffer_node_t *dummy;
    if (buffer == NULL) return SBUFFER_FAILURE;
    if (buffer->head == NULL) return SBUFFER_NO_DATA;
    *data = buffer->head->data;
    dummy = buffer->head;
    if (buffer->head == buffer->tail) // buffer has only one node
    {
        buffer->head = buffer->tail = NULL;
    } else  // buffer has many nodes empty
    {
        buffer->head = buffer->head->next;
    }
    free(dummy);
    return SBUFFER_SUCCESS;
}

int sbuffer_insert(sbuffer_t *buffer, sensor_data_t *data) { //reads_sem init() at end of insertion
    sbuffer_node_t *dummy;
    if (buffer == NULL) return SBUFFER_FAILURE;
    dummy = malloc(sizeof(sbuffer_node_t));
    if (dummy == NULL) return SBUFFER_FAILURE;
    dummy->data = *data;
    dummy->next = NULL;
    if (buffer->tail == NULL) // buffer empty (buffer->head should also be NULL
    {
        buffer->head = buffer->tail = dummy;
    } else // buffer not empty
    {   //tail 
        buffer->tail->next = dummy;
        buffer->tail = buffer->tail->next;
    }
    int semSucces = sem_init(&dummy->reads_sem,1,2); // __pshared = 1 : allow semaphore to be used by read&write process
    if(semSucces != 0) return SBUFFER_FAILURE;
    return SBUFFER_SUCCESS;
}

// TODO: make program threadsafe
// threadsafety as granular as possible
// mutex, semaphore, rwlock, barrier, condition var
// cancellation safety implementation p31
