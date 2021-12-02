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
#include <inttypes.h>
#include <sys/poll.h>

#define PORT 5678

void socket_free(void ** socket){
    free(*socket);
}

void * socket_copy(void * socket){
    poll_t * copy = malloc(sizeof(poll_t));
    copy->poll_fd = ((poll_t *)socket)->poll_fd;
    copy->socket = ((poll_t *)socket)->socket;
    return copy;
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
    dplist_t * sockets;
    sockets = dpl_create(socket_copy, socket_free, socket_compare);

    bool end = false;

    tcpsock_t * tcp_server, * client;
    poll_t server;
    pollfd_t fd_server;
    server.socket = tcp_server;
    server.poll_fd = fd_server;

    sockets = dpl_insert_at_index(sockets, &server,0,true);
    sensor_data_t data;
    int bytes, result; //these start variables copied from test_server template
    if (tcp_passive_open(&(server.socket), PORT) != TCP_NO_ERROR) exit(EXIT_FAILURE);
    do{
        poll_t * current_poll = dpl_get_element_at_index(sockets, 0);
        if(poll(&current_poll->poll_fd,1,0)>=0){
            printf("in poll\n");
            for (int i = 0; i< dpl_size(sockets); i++){ //go over each socket and read data
                printf("before for\n");
                current_poll = dpl_get_element_at_index(sockets, i);
                printf("in for\n"); //keep track of current poll
                //server checken en nieuwe connecties aanmaken, server staat vooraan in dplist
                if(i == 0){ //server node om nieuwe connecties te aanvaarden
                    tcp_wait_for_connection(current_poll->socket, &client);
                    poll_t client_copy;
                    client_copy.socket = client;
                    int pos = dpl_size(sockets)+1;
                    dpl_insert_at_index(sockets, &client_copy, pos, true);
                    printf("in server node\n");

                } else { //sockets die verbonden zijn overlopen
                    // read sensor ID
                    bytes = sizeof(data.id);
                    result = tcp_receive(client, (void *) &data.id, &bytes);
                    // read temperature
                    bytes = sizeof(data.value);
                    result = tcp_receive(client, (void *) &data.value, &bytes);
                    // read timestamp
                    bytes = sizeof(data.ts);
                    result = tcp_receive(client, (void *) &data.ts, &bytes);
                    if ((result == TCP_NO_ERROR) && bytes) {
                        printf("sensor id = %" PRIu16 " - temperature = %g - timestamp = %ld\n",
                        data.id, data.value,(long int) data.ts);
                    }
                }
            }
        }
        
    } while (end == false);
    

}
