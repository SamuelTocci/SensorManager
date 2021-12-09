/**
 * \author Samuel Tocci
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "dplist.h"

/*
 * definition of error codes
 * */
#define DPLIST_NO_ERROR 0
#define DPLIST_MEMORY_ERROR 1 // error due to mem alloc failure
#define DPLIST_INVALID_ERROR 2 //error due to a list operation applied on a NULL list 

#ifdef DEBUG
#define DEBUG_PRINTF(...) 									                                        \
        do {											                                            \
            fprintf(stderr,"\nIn %s - function %s at line %d: ", __FILE__, __func__, __LINE__);	    \
            fprintf(stderr,__VA_ARGS__);								                            \
            fflush(stderr);                                                                         \
                } while(0)
#else
#define DEBUG_PRINTF(...) (void)0
#endif


#define DPLIST_ERR_HANDLER(condition, err_code)                         \
    do {                                                                \
            if ((condition)) DEBUG_PRINTF(#condition " failed\n");      \
            assert(!(condition));                                       \
        } while(0)


/*
 * The real definition of struct list / struct node
 */

struct dplist_node {
    dplist_node_t *prev, *next;
    void *element;
};

struct dplist {
    dplist_node_t *head;

    void *(*element_copy)(void *src_element);

    void (*element_free)(void **element);

    int (*element_compare)(void *x, void *y);
};


dplist_t *dpl_create(// callback functions
        void *(*element_copy)(void *src_element),
        void (*element_free)(void **element),
        int (*element_compare)(void *x, void *y)
) {
    dplist_t *list;
    list = malloc(sizeof(struct dplist));
    DPLIST_ERR_HANDLER(list == NULL, DPLIST_MEMORY_ERROR);
    list->head = NULL;
    list->element_copy = element_copy;
    list->element_free = element_free;
    list->element_compare = element_compare;
    return list;
}

void dpl_free(dplist_t **list, bool free_element) {

    if(*list == NULL) return;
    dplist_node_t * overstep_node;
    if((*list)->head != NULL) {
        while((*list)->head->next != NULL){ //as long as list has 1 element
            if((*list)->head->next->next != NULL){
                overstep_node = (*list)->head->next->next;
            } else overstep_node = NULL;
            if(free_element){
                (*list)->element_free(&(*list)->head->next->element);
                //TODO: dit deel werkt niet
            }
            (*list)->head->next->prev = NULL;
            (*list)->head->next->element = NULL;
            free((*list)->head->next);
            (*list)->head->next = overstep_node;
        }
    }
    (*list)->element_compare = NULL;
    (*list)->element_free = NULL;
    (*list)->element_copy = NULL;
    (*list)->head = NULL;
    free(*list);
    *list = NULL;
}

dplist_t *dpl_insert_at_index(dplist_t *list, void *element, int index, bool insert_copy) {

    dplist_node_t *ref_at_index, *list_node;
    if (list == NULL) return NULL;

    list_node = malloc(sizeof(dplist_node_t));
    DPLIST_ERR_HANDLER(list_node == NULL, DPLIST_MEMORY_ERROR);
    if (insert_copy){
        list_node->element = list->element_copy(element);
    } else {
        list_node->element = element;
    }
    // pointer drawing breakpoint
    if (list->head == NULL) { // covers case 1
        list_node->prev = NULL;
        list_node->next = NULL;
        list->head = list_node;
        // pointer drawing breakpoint
    } else if (index <= 0) { // covers case 2
        list_node->prev = NULL;
        list_node->next = list->head;
        list->head->prev = list_node;
        list->head = list_node;
        // pointer drawing breakpoint
    } else {
        ref_at_index = dpl_get_reference_at_index(list, index);
        assert(ref_at_index != NULL);
        // pointer drawing breakpoint
        if (index < dpl_size(list)) { // covers case 4
            list_node->prev = ref_at_index->prev;
            list_node->next = ref_at_index;
            ref_at_index->prev->next = list_node;
            ref_at_index->prev = list_node;
            // pointer drawing breakpoint
        } else { // covers case 3
            assert(ref_at_index->next == NULL);
            list_node->next = NULL;
            list_node->prev = ref_at_index;
            ref_at_index->next = list_node;
            // pointer drawing breakpoint
        }
    }
    return list;
}

dplist_t *dpl_remove_at_index(dplist_t *list, int index, bool free_element) {

    if (list == NULL || dpl_size(list) == 0) return list;//if there is nothing to delete, don't delete

    dplist_node_t * list_node = dpl_get_reference_at_index(list, index);

    if(index <=0){
        if(list_node->next != NULL){ //if list bigger than 1
            //link head with next node
            list_node->next->prev = list->head;
            list->head = list_node->next;
            //handle cut out node
            if(free_element) list->element_free(list_node->element);
            list_node->prev = NULL;
            list_node->next = NULL;
            list_node->element = NULL;
            list_node = NULL;
        } else{ //if list only 1 element
            list->head->next = NULL;
            //handle cut out node
            if(free_element) list->element_free(list_node->element);
            list_node->prev = NULL;
        }
    }
    else{ //index > 0
        //TODO: index >0 bijvoegen
        if(list_node->next == NULL){ //node is at end of list
            list_node->prev->next = NULL;
            //handle cut out node
            list_node->prev = NULL;
            if(free_element) list->element_free(list_node->element);
            list_node->element = NULL;
            list_node = NULL;
        } else { //node somewhere inside of list
            list_node->prev->next = list_node->next;
            list_node->next->prev = list_node->prev;
            //handle cut out node
            if(free_element) list->element_free(list_node->element);
            list_node->prev = NULL;
            list_node->next = NULL;
            list_node->element = NULL;
            list_node = NULL;
        }
    }
    free(list_node);
    return list;
}

int dpl_size(dplist_t *list) {
    if(list == NULL) return -1;
    if (list->head == NULL) return 0;
    int counter = 1; 
    dplist_node_t *el;
    el = list->head;
    while (el->next != NULL)
    {
        el = el->next;
        counter ++;
    }
    return counter;
}

void *dpl_get_element_at_index(dplist_t *list, int index) {
    dplist_node_t *curr_node = list->head;
    // if(curr_node->next != NULL) curr_node = curr_node->next; //goto index 0 if there is at least 1 element
    for (int i = 0; i < index; i++){
        if(curr_node->next != NULL) curr_node = curr_node->next;
    }
    return curr_node->element;
}

int dpl_get_index_of_element(dplist_t *list, void *element) {
    if(list == NULL)return -1;
    dplist_node_t *curr_node = list->head;
    for (int index = 0; index < dpl_size(list)-1 ; index++){
        curr_node = curr_node->next;
        void *curr_element = curr_node->element;
        if(list->element_compare(curr_element, element) == 0){ //==0 betekend dat ze gelijk zijn
            return index;
        }
    }
    return -1;
}

dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index) {
    dplist_node_t *curr_node = list->head;
    // if(curr_node->next != NULL) curr_node = curr_node->next; //goto index 0 if there is at least 1 element
    for (int i = 0; i < index; i++){
        if(curr_node->next != NULL) curr_node = curr_node->next;
    }
    return curr_node;
}

void *dpl_get_element_at_reference(dplist_t *list, dplist_node_t *reference) {
    return reference->element;
}





