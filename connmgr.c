/**
 * \author Samuel Tocci
 */
#define _GNU_SOURCE

#include "connmgr.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include "lib/dplist.h"
#include "lib/tcpsock.h"
#include <sys/poll.h>

#define PORT 5678

dplist_t * sockets;

void socket_free(void ** socket){
    free(*socket);
}

void * socket_copy(void * socket){
    return 0;
}

int socket_compare(void * socket1, void * socket2){
    return 0;
}

/**
 * SELECT vs POLL vs EPOLL!
 * - select gaat door lijst van fileds 1,2,...,200 tot hij connectie vind (niet performant, O(fd_id)) 
 * - poll houdt open fd bij dus rechtstreeks naar open connectie (nog steeds O(#connections))
 * - epoll is 0(1) bij edge trigger maar in mijn geval gebruik ik level trigger dus zelfde als poll
 * epoll gebruiken zou meer problemen opleveren zonder performance gain
 * CONCLUSIE: poll gebruiken
*/


void con_listen(){
    FILE * file = fopen(OUTPUT_NAME, "w"); //write to output file -> to datastream in final_assignment
    sockets = dpl_create(socket_copy, socket_free, socket_compare);

    bool end = false;

    tcpsock_t * server, * client;
    sensor_data_t data;
    int bytes, result;
    int socket_counter = 0; //these start variables copied from test_server template
    if (tcp_passive_open(&server, PORT) != TCP_NO_ERROR) exit(EXIT_FAILURE);
    do{
        socket_counter = dpl_size(sockets);
        for (int i = 0; i < socket_counter; i++){ //go over each socket and read data
            poll_t * current_poll = dpl_get_element_at_index(sockets, i); //keep track of current poll
            //server checken en nieuwe connecties aanmaken
            if(i == 0){
                tcp_wait_for_connection(server, &client);
            }
            //sockets die verbonden zijn overlopen
        }
        
    } while (end == false);
    

}
