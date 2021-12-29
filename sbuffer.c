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
    int reads_left;                     /**< an integer count to keep track of amount of reads left */
    pthread_mutex_t reads_left_mutex;   /**< a mutex to lock the reads_left when modifying the value */
} sbuffer_node_t;

/**
 * a structure to keep track of the buffer
 */
struct sbuffer {
    sbuffer_node_t *head;         /**< a pointer to the first node in the buffer */
    sbuffer_node_t *tail;         /**< a pointer to the last node in the buffer */
    sbuffer_node_t *storage_curr;      /**< a pointer to the node dbmngr is currently reading */
    sbuffer_node_t *data_curr;      /**< a pointer to the node dbmngr is currently reading */
    pthread_rwlock_t * tail_rwlock; /**< a rwLock for the tail of the sbuffer */
};

/** Mutex for the glorified integer count
 *  tail lock for writing process -> no readings possible during write/insertion into sbuffer
 */

int sbuffer_init(sbuffer_t **buffer) { //thread safety not needed, only used when starting program
    *buffer = malloc(sizeof(sbuffer_t));
    if (*buffer == NULL) return SBUFFER_FAILURE;
    (*buffer)->head = NULL;
    (*buffer)->tail = NULL;
    (*buffer)->storage_curr = NULL;
    (*buffer)->data_curr = NULL;
    (*buffer)->tail_rwlock = malloc(sizeof(pthread_rwlock_t));
    pthread_rwlockattr_t * attr = malloc(sizeof(pthread_rwlockattr_t));
    pthread_rwlockattr_init(attr);
    pthread_rwlockattr_setpshared(attr, PTHREAD_PROCESS_SHARED);
    pthread_rwlock_init((*buffer)->tail_rwlock, attr);
    free(attr);
    free((*buffer)->tail_rwlock);
    return SBUFFER_SUCCESS;
}

int sbuffer_free(sbuffer_t **buffer) { //thread safety not needed, only used when shutting down program
    if ((buffer == NULL) || (*buffer == NULL)) {
        return SBUFFER_FAILURE;
    }
    while ((*buffer)->head) {
        sbuffer_node_t *dummy;
        dummy = (*buffer)->head;
        (*buffer)->head = (*buffer)->head->next;
        free(dummy);
    }
    free(*buffer);
    *buffer = NULL;
    return SBUFFER_SUCCESS;
}

int sbuffer_remove(sbuffer_t *buffer, sensor_data_t_packed *data) {
    if (buffer == NULL) return SBUFFER_FAILURE;
    if (buffer->head == NULL) return SBUFFER_NO_DATA;
    sbuffer_node_t *dummy;
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
    /** Thread Safety **/
    pthread_mutex_init(&dummy->reads_left_mutex, NULL);
    dummy->reads_left = 2;

    pthread_rwlock_wrlock(buffer->tail_rwlock);// Write to the tail (shared in sbuffer with all threads)
    if (buffer->tail == NULL) // buffer empty (buffer->head should also be NULL
    {
        buffer->head = buffer->tail = dummy;
    } else // buffer not empty
    {   //tail 
        buffer->tail->next = dummy;
        buffer->tail = buffer->tail->next;
    }
    pthread_rwlock_unlock(buffer->tail_rwlock);
    return SBUFFER_SUCCESS;
}

sensor_data_t_packed * sbuffer_next(sbuffer_t * sbuffer, char process){
    if(sbuffer->head == NULL) return NULL;
    // sensor_data_t_packed * data = malloc(sizeof(sensor_data_t_packed));

    switch (process){
    case 0:
        if(sbuffer->data_curr == NULL){
            sbuffer->data_curr = sbuffer->head;
        } else {
            if(sbuffer->data_curr->next != NULL)sbuffer->data_curr = sbuffer->data_curr->next;
            else return NULL;
            pthread_mutex_lock(&sbuffer->data_curr->reads_left_mutex);
            sbuffer->data_curr->reads_left --;
            // printf("DATA: %i\n",sbuffer->data_curr->reads_left );
            // if(sbuffer->data_curr->reads_left <= 0)printf("reads op 0\n");// sbuffer_remove(sbuffer, data);
            pthread_mutex_unlock(&sbuffer->data_curr->reads_left_mutex);

            // printf("%f\n", sbuffer->data_curr->data.value );
            return &sbuffer->data_curr->data;
        }
        break;

    case 1:
        if(sbuffer->storage_curr == NULL){
            sbuffer->storage_curr = sbuffer->head;
        } else {
            if(sbuffer->storage_curr->next != NULL)sbuffer->storage_curr = sbuffer->storage_curr->next;
            else return NULL;
            pthread_mutex_lock(&sbuffer->storage_curr->reads_left_mutex);
            sbuffer->storage_curr->reads_left --;
            // printf("STORAGE: %i\n",sbuffer->storage_curr->reads_left ); 
            // if(sbuffer->storage_curr->reads_left <= 0) printf("reads op 1\n");//sbuffer_remove(sbuffer, data);
            pthread_mutex_unlock(&sbuffer->storage_curr->reads_left_mutex);

            // printf("%f\n",sbuffer->storage_curr->data.value );
            return &sbuffer->storage_curr->data;
        }
        break;
    }
    return NULL;
}

// TODO: make program threadsafe
// threadsafety as granular as possible
// mutex, semaphore, rwlock, barrier, condition var
// cancellation safety implementation p31

