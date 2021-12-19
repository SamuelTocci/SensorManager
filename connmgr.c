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
#include <unistd.h>

/**
 * SELECT vs POLL vs EPOLL!
 * - select gaat door lijst van fileds 1,2,...,200 tot hij connectie vind (niet performant, O(fd_id)) 
 * - poll houdt open fd bij dus rechtstreeks naar open connectie (nog steeds O(#connections))
 * - epoll is 0(1) bij edge trigger maar in mijn geval gebruik ik level trigger dus zelfde als poll
 * epoll gebruiken zou meer problemen opleveren zonder performance gain
 * CONCLUSIE: poll gebruiken
*/


void socket_free(void ** socket){
    free(*socket);
}

void * socket_copy(void * socket){
    tcpsock_t * copy = (tcpsock_t *) malloc(sizeof(tcpsock_t*));
    copy->sd = ((tcpsock_t *)socket)->sd;
    copy->cookie = ((tcpsock_t *)socket)->cookie;
    copy->ip_addr = ((tcpsock_t *)socket)->ip_addr;
    copy->port = ((tcpsock_t *)socket)->port;
    return copy;
}

int socket_compare(void * socket1, void * socket2){
    return 0;
}


void connmgr_listen(int port_nr){
    FILE * file = fopen(OUTPUT_NAME, "w"); //write to output file -> to datastream in final_assignment
    dplist_t * sockets;
    sockets = dpl_create(socket_copy, socket_free, socket_compare);

    tcpsock_t * tcp_server, * client;
    pollfd_t * poll_fds = malloc(sizeof(pollfd_t));
    
    if (tcp_passive_open(&tcp_server, port_nr) != TCP_NO_ERROR) exit(EXIT_FAILURE);

    tcp_get_sd(tcp_server, &(poll_fds[0].fd));
    poll_fds[0].events = POLLIN;
    int poll_result, bytes, tcp_result;
    int conn_count = 0;
    printf("-[server]- started\n");
    do{
        poll_result = poll(poll_fds, conn_count +1,5000);
        if(poll_result>0){
            if(poll_fds[0].revents & POLLIN){
                tcp_wait_for_connection(tcp_server, &client);
                
                poll_fds = (pollfd_t *) realloc(poll_fds, sizeof(pollfd_t)*(conn_count+1));
                tcp_get_sd(client, &(poll_fds[conn_count+1].fd));
                poll_fds[conn_count+1].events = POLLIN;
                dpl_insert_at_index(sockets, client, conn_count + 1, true);
                conn_count++;
                printf("-[server]- new socket connected\n");
            }
        }
        for (int i = 0; i < conn_count; i++){
            if(poll_fds[i+1].revents & POLLIN){
                sensor_data_t_packed data;
                tcpsock_t * curr_client = dpl_get_element_at_index(sockets, i);

                //printf("-client- reading data\n");
                // read sensor ID
                bytes = sizeof(data.id);
                tcp_result = tcp_receive(curr_client, (void *) &data.id, &bytes);
                // read temperature
                bytes = sizeof(data.value);
                tcp_result = tcp_receive(curr_client, (void *) &data.value, &bytes);
                // read timestamp
                bytes = sizeof(data.ts);
                tcp_result = tcp_receive(curr_client, (void *) &data.ts, &bytes);
                if ((tcp_result == TCP_NO_ERROR) && bytes) {
                    // printf("sensor id = %i - temperature = %g - timestamp = %li\n",
                    // data.id, data.value, data.ts);
                    fwrite(&data, sizeof(sensor_data_t_packed),1,file);
                }
            }
        }
    }while(1);
}
