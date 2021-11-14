/**
 * \author Samuel Tocci
 */

#define _GNU_SOURCE

#include "dplist.h"
#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef struct {
    int id;
    char* name;
} my_element_t;

void* element_copy(void * element);
void element_free(void ** element);
int element_compare(void * x, void * y);

void * element_copy(void * element) {
    my_element_t* copy = malloc(sizeof (my_element_t));
    char* new_name;
    asprintf(&new_name,"%s",((my_element_t*)element)->name);
    assert(copy != NULL);
    copy->id = ((my_element_t*)element)->id;
    copy->name = new_name;
    return (void *) copy;
}

void element_free(void ** element) {
    free((((my_element_t*)*element))->name);
    free(*element);
    *element = NULL;
}

int element_compare(void * x, void * y) {
    return ((((my_element_t*)x)->id < ((my_element_t*)y)->id) ? -1 : (((my_element_t*)x)->id == ((my_element_t*)y)->id) ? 0 : 1);
}

void setup(void) {
    // Implement pre-test setup
}

void teardown(void) {
    // Implement post-test teardown
}
START_TEST(test_ListFree)
    {
        int test = 5;
        int * test_ptr = &test;
        // Test free NULL, don't use callback
        dplist_t *list = NULL;
        dpl_free(&list, false);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

        // Test free NULL, use callback
        list = NULL;
        dpl_free(&list, true);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

        // Test free empty list, don't use callback
        list = dpl_create(element_copy, element_free, element_compare);
        dpl_free(&list, false);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

        // Test free empty list, use callback
        list = dpl_create(element_copy, element_free, element_compare);
        dpl_free(&list, true);
        ck_assert_msg(list == NULL,"Failure: expected result to be NULL" );

        // TODO : Test free with one element, also test if inserted elements are set to NULL
        list = dpl_create(element_copy, element_free, element_compare);
        my_element_t *dummy = malloc(sizeof(my_element_t));
        dpl_insert_at_index(list,dummy,-1, false);
        dpl_free(&list, true);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");

        // TODO : Test free with multiple elements, also test if inserted elements are set to NULL
        list = dpl_create(element_copy, element_free, element_compare);
        my_element_t *dummy1 = malloc(sizeof(my_element_t));
        my_element_t *dummy2 = malloc(sizeof(my_element_t));
        my_element_t *dummy3 = malloc(sizeof(my_element_t));
        dpl_insert_at_index(list, dummy1, 0, false);
        dpl_insert_at_index(list, dummy2, 1, false);
        dpl_insert_at_index(list, dummy3, 2, false);
        dpl_free(&list, true);
        ck_assert_msg(list == NULL, "Failure: expected result to be NULL");
    }
END_TEST

START_TEST(test_ListInsertAtIndexListNULL)
    {
        int test = 5;
        int * test_ptr = &test;
        // Test inserting at index -1
        dplist_t *result = dpl_insert_at_index(NULL, test_ptr, -1, false);
        ck_assert_msg(result == NULL, "Failure: expected list to be NULL");
        dpl_free(&result,true);

        // TODO : Test inserting at index 0
        result = dpl_insert_at_index(NULL, test_ptr, 0, false);
        ck_assert_msg(result == NULL, "Failure: expected list to be NULL");
        dpl_free(&result,true);

        // TODO : Test inserting at index 99
        result = dpl_insert_at_index(NULL, test_ptr, 99, false);
        ck_assert_msg(result == NULL, "Failure: expected list to be NULL");
        dpl_free(&result,true);
    }
END_TEST

//START_TEST(test_nameOfYourTest)
//  Add other testcases here...
//END_TEST

int main(void) {
    Suite *s1 = suite_create("LIST_EX3");
    TCase *tc1_1 = tcase_create("Core");
    SRunner *sr = srunner_create(s1);
    int nf;

    suite_add_tcase(s1, tc1_1);
    tcase_add_checked_fixture(tc1_1, setup, teardown);
    tcase_add_test(tc1_1, test_ListFree);
    tcase_add_test(tc1_1,test_ListInsertAtIndexListNULL);
    // Add other tests here...

    srunner_run_all(sr, CK_VERBOSE);

    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return nf == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
