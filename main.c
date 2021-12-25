#define _GNU_SOURCE


#include <stdlib.h>
#include <stdio.h>
#include <unistd.h> 
#include <signal.h>
#include <pthread.h>
#include "sbuffer.h"
#include "config.h"

int main (void){
    //setup
    sbuffer_t * testBuff;
    int resultCodeInit = sbuffer_init(&testBuff);
    printf("%i\n",resultCodeInit);

    //main program
    pid_t rw_pid = fork();

    if (rw_pid == 0){ //child
        //reading out of sbuffer, deleting node when 2 readers done
        //2 reader threads

    } else { //parent
        //insertion in sbuffer
        sensor_data_t * testData;
        testData->id = 15;
        testData->value = 20;
        do
        {
            testData->ts=time(NULL);
            sbuffer_insert(testBuff, testData);
        } while (1);
        

        kill(getpid(), SIGKILL);
    }
    

    //teardown
    int resultCodeEnd = sbuffer_free(&testBuff);
    printf("%i\n",resultCodeEnd);
    return 0;
}