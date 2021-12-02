#define _GNU_SOURCE

#include "config.h"
#include "sensor_db.h"
#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>

void setup(void) {}

void teardown(void) {}

START_TEST(test_init_connect)
{
    DBCONN *result = init_connection(1);
    ck_assert_msg(result != NULL, "Error: expected result to not be NULL");
    disconnect(result); //to avoid mem leak
}
END_TEST

START_TEST(test_insert_sensor)
{
    DBCONN *result = init_connection(0);
    
    int check = insert_sensor(result, 5, 188, 27758);
    ck_assert_msg(check == 0, "Error: expected result to to be 0");

}
END_TEST

START_TEST(test_pipe)
{
    DBCONN *result = init_connection(0);
    
    int check = insert_sensor(result, 5, 188, 27758);
    ck_assert_msg(check == 0, "Error: expected result to to be 0");

}
END_TEST


int main(void){
    Suite *s1 = suite_create("LIST_EX3");
    TCase *tc1_1 = tcase_create("Core");
    SRunner *sr = srunner_create(s1);
    int nf;

    suite_add_tcase(s1, tc1_1);
    tcase_add_checked_fixture(tc1_1, setup, teardown);
    tcase_add_test(tc1_1, test_init_connect);
    // tcase_add_test(tc1_1, test_insert_sensor);
    // tcase_add_test(tc1_1, test_pipe);

    // Add other tests here...

    srunner_run_all(sr, CK_VERBOSE);

    nf = srunner_ntests_failed(sr);
    srunner_free(sr);
    return nf == 0 ? EXIT_SUCCESS : EXIT_FAILURE;


}

// int print_row(void* arg1,int num_columns, char * column_value, char* column_name){
//     int i;
//     for(i =1;i< num_columns; i++){
//         printf("[%d=%p]\t",column_name[i],column_value[i] ? column_value[i] : NULL);
//     }
//     printf("\n");
//     return 0;
// }