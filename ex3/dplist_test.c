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
    assert(copy != NULL);
    copy->id = ((my_element_t*)element)->id;
    copy->name = ((my_element_t*)element)->name;
    return (void *) copy;
}

void element_free(void ** element) {
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
        my_element_t * test_ptr0 = malloc(sizeof(my_element_t));
        test_ptr0->id = 5;
        test_ptr0->name = "ptr 0";
        my_element_t * test_ptr1 = malloc(sizeof(my_element_t));
        test_ptr1->name = "ptr 1";
        dplist_t * list = dpl_create(element_copy, element_free, element_compare);
        dpl_insert_at_index(list, test_ptr1, -1, false);
        dpl_insert_at_index(list, test_ptr0, -1, false);
        dpl_insert_at_index(list, test_ptr0, -1, true);
        dpl_remove_at_index(list,2,true);

        int size = dpl_size(list);
        printf("%i\n",size);

        my_element_t * result_ptr = dpl_get_element_at_index(list,-1);
        printf("%s\n",result_ptr->name);

        result_ptr = dpl_get_element_at_reference(list,dpl_get_reference_at_index(list, 0));
        printf("%s\n",result_ptr->name);

        dpl_free(&list,true);
        ck_assert_msg(size == 1, "Failure: expected list to be 1");
    }
END_TEST

START_TEST(test_ListRemoveAtIndexListNull)
    {
        dplist_t * list = NULL;
        dplist_t * result;
        result = dpl_remove_at_index(list, -1, false);
        ck_assert_msg(result == NULL, "Failure: expected list to be NULL");

        result = dpl_remove_at_index(list, -1, false);
        ck_assert_msg(result == NULL, "Failure: expected list to be NULL");

        result = dpl_remove_at_index(list, 0, false);
        ck_assert_msg(result == NULL, "Failure: expected list to be NULL");

        result = dpl_remove_at_index(list, 99, false);
        ck_assert_msg(result == NULL, "Failure: expected list to be NULL");

        result = dpl_remove_at_index(list, 1, true);
        ck_assert_msg(result == NULL, "Failure: expected list to be NULL");
    }
END_TEST

START_TEST(test_ListRemoveAtIndexListEmpty)
    {
        dplist_t * list = dpl_create(element_copy, element_free, element_compare);
        dplist_t * result;
        result = dpl_remove_at_index(list, -1, false);
        ck_assert_msg(result == list, "Failure: expected list to remain the same");

        result = dpl_remove_at_index(list, 0, false);
        ck_assert_msg(result == list, "Failure: expected list to remain the same");

        result = dpl_remove_at_index(list, 99, false);
        ck_assert_msg(result == list, "Failure: expected list to remain the same");

        result = dpl_remove_at_index(list, 1, true);
        ck_assert_msg(result == list, "Failure: expected list to remain the same");
    }
END_TEST

START_TEST(test_ListRemoveAtIndexListOne)
    {
        dplist_t * listCompare = dpl_create(element_copy, element_free, element_compare);
        dplist_t * result;

        dplist_t * list = dpl_create(element_copy, element_free, element_compare);
        my_element_t *dummy = malloc(sizeof(my_element_t));
        dpl_insert_at_index(list,dummy,-1, false);
        result = dpl_remove_at_index(list, -1, false);
        ck_assert_msg(result == listCompare, "Failure: expected list to be empty");

        // list = dpl_create(element_copy, element_free, element_compare);
        // dummy = malloc(sizeof(my_element_t));
        // dpl_insert_at_index(list,dummy,-1, false);
        // result = dpl_remove_at_index(list, 0, false);
        // ck_assert_msg(result == listCompare, "Failure: expected list to be empty");

        // list = dpl_create(element_copy, element_free, element_compare);
        // dummy = malloc(sizeof(my_element_t));
        // dpl_insert_at_index(list,dummy,-1, false);
        // result = dpl_remove_at_index(list, 99, false);
        // ck_assert_msg(result == listCompare, "Failure: expected list to be empty");

        // list = dpl_create(element_copy, element_free, element_compare);
        // dummy = malloc(sizeof(my_element_t));
        // dpl_insert_at_index(list,dummy,-1, false);
        // result = dpl_remove_at_index(list, 1, true);
        // ck_assert_msg(result == listCompare, "Failure: expected list to be empty");
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
    // tcase_add_test(tc1_1, test_ListInsertAtIndexListNULL);
    // tcase_add_test(tc1_1, test_ListRemoveAtIndexListNull);
    // tcase_add_test(tc1_1, test_ListRemoveAtIndexListEmpty);
    // tcase_add_test(tc1_1, test_ListRemoveAtIndexListOne);
    // Add other tests here...

    srunner_run_all(sr, CK_VERBOSE);

    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return nf == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
