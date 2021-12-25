#define _GNU_SOURCE

#include "config.h"
#include "sensor_db.h"
#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <assert.h>
#include <unistd.h>

void setup(void) {}

void teardown(void) {}

// START_TEST(test_init_connect)
// {
//     DBCONN *result = init_connection(1);
//     ck_assert_msg(result != NULL, "Error: expected result to not be NULL");
//     disconnect(result); //to avoid mem leak
// }
// END_TEST

// START_TEST(test_insert_sensor)
// {
//     DBCONN *result = init_connection(0);
    
//     int check = insert_sensor(result, 5, 188, 27758);
//     ck_assert_msg(check == 0, "Error: expected result to to be 0");

// }
// END_TEST

// START_TEST(test_fifo)
// {
//     int pid = fork();
// 	if(pid == 0){//child process, log mngr
//         do{
//             read_from_fifo();
//         } while (1);
// 		exit(EXIT_SUCCESS);
		
// 	} else { //parent process, main
// 		DBCONN *result = init_connection(1);
//         insert_sensor(result, 5, 188, 27758);
//         insert_sensor(result, 17, 18.62, 27758);
//         insert_sensor(result, 17, 18.20, 27790);

//         disconnect(result);
//         ck_assert_msg(result != NULL, "Error: expected result to to be 0");
//         exit(EXIT_SUCCESS);
//     }


// }
// END_TEST


int main(void){
    int pid = fork();
	if(pid == 0){//child process, log mngr
        do{
            read_from_fifo();
        } while (1);
		exit(EXIT_SUCCESS);
		
	} else { //parent process, main
		DBCONN *result = init_connection(1);
        insert_sensor(result, 5, 188, 27758);
        insert_sensor(result, 17, 18.62, 27758);
        insert_sensor(result, 17, 18.20, 27790);

        disconnect(result);
        exit(EXIT_SUCCESS);
    }


}