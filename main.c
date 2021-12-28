#include "config.h"
#include "sbuffer.h"
#include "sensor_db.h"
#include "datamgr.h"
#include "connmgr.h"
#include <pthread.h>
#include <unistd.h>

sbuffer_t * sbuffer;

void connmgr_routine(void * port_number){
    printf("%i\n",atoi(port_number) );
    connmgr_listen(atoi(port_number), sbuffer);
    connmgr_free();
    printf("connmgr discconnect\n");
}

void datamgr_routine(){
    FILE * map = fopen("room_sensor.map", "r");

    datamgr_parse_sensor_files(map, sbuffer);
    datamgr_free();
    printf("data disconnect\n");
}

void db_routine(){
    DBCONN * sqlite = init_connection(1);
        
    insert_sensor_from_file(sqlite, sbuffer);

    disconnect(sqlite);
}

int main(int argc, char const *argv[]){
    sbuffer_init(&sbuffer);

    int pid = fork();
	if(pid == 0){//child process, log mngr
        do{
            read_from_fifo();
        } while (1);
		
	} else { //parent process, main

        pthread_t * connmgr_pthread = malloc(sizeof(pthread_t));
        pthread_create(connmgr_pthread, NULL,(void *) &connmgr_routine, (void *) argv[1]);
        pthread_t * datamgr_pthread = malloc(sizeof(pthread_t));
        pthread_create(datamgr_pthread, NULL,(void *) &datamgr_routine, NULL);
        pthread_t * db_pthread = malloc(sizeof(pthread_t));
        pthread_create(db_pthread, NULL,(void *) &db_routine, NULL);

        pthread_join(*connmgr_pthread, NULL);
        pthread_join(*datamgr_pthread, NULL);
        pthread_join(*db_pthread, NULL);
    }

    return 0;
}
