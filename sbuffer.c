/**
 * \author Samuel Tocci
 */
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include "sbuffer.h"
#include <semaphore.h>
#include <pthread.h>

/**
 * basic node for the buffer, these nodes are linked together to create the buffer
 */
typedef struct sbuffer_node {
    struct sbuffer_node *next;           /**< a pointer to the next node */
    sensor_data_t_packed data;                  /**< a structure containing the data */
    sem_t reads_sem;                     /**< a semaphore to keep track of amount of reads done */
} sbuffer_node_t;

/**
 * a structure to keep track of the buffer
 */
struct sbuffer {
    sbuffer_node_t *head;         /**< a pointer to the first node in the buffer */
    sbuffer_node_t *tail;         /**< a pointer to the last node in the buffer */
    pthread_rwlock_t * tail_rwlock; /**< a rwLock for the tail of the sbuffer */
};

/** Using semaphore = 2 for amount of readings done -> when 0 delete node 
 *  tail lock for writing process -> no readings possible during write/insertion into sbuffer
 */

int sbuffer_init(sbuffer_t **buffer) { //thread safety not needed, only used when starting program
    *buffer = malloc(sizeof(sbuffer_t));
    if (*buffer == NULL) return SBUFFER_FAILURE;
    (*buffer)->head = NULL;
    (*buffer)->tail = NULL;
    (*buffer)->tail_rwlock = malloc(sizeof(pthread_rwlock_t));
    pthread_rwlockattr_t * attr = malloc(sizeof(pthread_rwlockattr_t));
    pthread_rwlockattr_init(attr);
    pthread_rwlockattr_setpshared(attr, PTHREAD_PROCESS_SHARED);
    pthread_rwlock_init((*buffer)->tail_rwlock, attr);
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

int sbuffer_remove(sbuffer_t *buffer, sensor_data_t_packed *data) {
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

int sbuffer_insert(sbuffer_t *buffer, sensor_data_t_packed *data) { //reads_sem init() at end of insertion
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

sensor_data_t_packed * sbuffer_head(sbuffer_t * sbuffer){
    if(sbuffer->head != NULL){
        sensor_data_t_packed * head_data = malloc(sizeof(sensor_data_t_packed*));
        *head_data = (sbuffer->head)->data;
        return head_data;
    }
    return NULL;
}

// TODO: make program threadsafe
// threadsafety as granular as possible
// mutex, semaphore, rwlock, barrier, condition var
// cancellation safety implementation p31

