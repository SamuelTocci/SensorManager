/**
 * \author Samuel Tocci
 */

#define _GNU_SOURCE

#include <stdio.h>
#include "lib/dplist.h"
#include <stdlib.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include "datamgr.h"
#include "config.h"
#include "lib/dplist.h"
#include <check.h>
#include "datamgr.h"


void setup(void) {}

void teardown(void) {}

START_TEST(test_readFile)
{
    FILE * data_file = fopen("sensor_data", "r");
    FILE * map_file = fopen("room_sensor.map", "r");
    
    printf("readfile print\n");
    datamgr_parse_sensor_files(map_file, data_file);

    datamgr_free();

    fclose(data_file);
    fclose(map_file);
}
END_TEST

//START_TEST(test_readFile)
//  Add other testcases here...
//END_TEST


int main(void){
    Suite *s1 = suite_create("LIST_EX3");
    TCase *tc1_1 = tcase_create("Core");
    SRunner *sr = srunner_create(s1);
    int nf;

    suite_add_tcase(s1, tc1_1);
    tcase_add_checked_fixture(tc1_1, setup, teardown);
    tcase_add_test(tc1_1, test_readFile);
    // Add other tests here...

    srunner_run_all(sr, CK_VERBOSE);

    nf = srunner_ntests_failed(sr);
    srunner_free(sr);
    printf("main print\n");
    return nf == 0 ? EXIT_SUCCESS : EXIT_FAILURE;

}