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

    int size = dpl_size(*list);
    if(free_element == true){
        dplist_node_t *last = dpl_get_reference_at_index(*list,size);
        dplist_node_t *cur_free;
        if(last == NULL){
            free(*list);
            *list = NULL;
            return;
            }
        while (last->prev != NULL){
            cur_free = last;
            last = last->prev;
            (*list)->element_free(cur_free->element);
            free(cur_free);
        }
        (*list)->element_free(cur_free->element);
        free(last);
        free(*list);
        last = NULL;
        *list = NULL;
        return;
    }
    if(free_element == false){
        dplist_node_t *last = dpl_get_reference_at_index(*list,size);
        dplist_node_t *cur_free;
        if(last == NULL){
            free(*list);
            *list = NULL;
            return;
            }
        while (last->prev != NULL){
            cur_free = last;
            last = last->prev;
            free(cur_free);
        }
        free(last);
        free(*list);
        last = NULL;
        *list = NULL;
        return;
    }
}

dplist_t *dpl_insert_at_index(dplist_t *list, void *element, int index, bool insert_copy) {

    dplist_node_t *ref_at_index, *list_node;
    if (list == NULL) return NULL;

    list_node = malloc(sizeof(dplist_node_t));
    DPLIST_ERR_HANDLER(list_node == NULL, DPLIST_MEMORY_ERROR);
    list_node->element = element;
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
    //TODO: code aanpassen

}

dplist_t *dpl_remove_at_index(dplist_t *list, int index, bool free_element) {

    //TODO: add your code here

}

int dpl_size(dplist_t *list) {
    if(list == NULL) return -1;
    int counter = 0;
    dplist_node_t *el;
    el = list->head;//checking if it is null to vermijd nullpointer(segmentation fault in C)
    while (el != NULL)
    {
        counter ++;
        el = el->next;
    }
    return counter;//TODO: add your code here
}

void *dpl_get_element_at_index(dplist_t *list, int index) {

    //TODO: add your code here

}

int dpl_get_index_of_element(dplist_t *list, void *element) {

    //TODO: add your code here

}

dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index) {
    int count;
    dplist_node_t *dummy;
    DPLIST_ERR_HANDLER(list == NULL, DPLIST_INVALID_ERROR);

    if (list->head == NULL) return NULL;

    for (dummy = list->head, count = 0; dummy->next != NULL; dummy = dummy->next, count++) {
        if (count >= index) return dummy;
    }
    return dummy;
}

void *dpl_get_element_at_reference(dplist_t *list, dplist_node_t *reference) {

    //TODO: add your code here

}


