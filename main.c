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

void* element_copy(void * element);
void element_free(void ** element);
int element_compare(void * x, void * y);

void * element_copy(void * element) {
    // sensor_data_t* copy = malloc(sizeof (sensor_data_t));
    // sensor_id_t new_id;
    // asprintf(&new_id,"%s",((sensor_data_t*)element)->id);
    // assert(copy != NULL);
    // copy->id = ((sensor_data_t*)element)->id;
    // copy->id = new_id;
    // return (void *) copy;
    void * a;
    return a;
    //TODO: code aanpassen naar sensor_data_t struct
}

void element_free(void ** element) {
    free((((sensor_data_t*)*element))->id);
    free((((sensor_data_t*)*element))->value);
    free((((sensor_data_t*)*element))->ts);
    free(*element);
    *element = NULL;
}

int element_compare(void * x, void * y) {
    return ((((sensor_data_t*)x)->id < ((sensor_data_t*)y)->id) ? -1 : (((sensor_data_t*)x)->id == ((sensor_data_t*)y)->id) ? 0 : 1);
}

void setup(void) {}

void teardown(void) {}

START_TEST(test_readFile)
{
    FILE * data_file = fopen("sensor_data", "r");
    FILE * map_file = fopen("room_sensor.map", "r");
    
    datamgr_parse_sensor_files(map_file, data_file);

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

    return nf == 0 ? EXIT_SUCCESS : EXIT_FAILURE;

}