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
#include "sbuffer.h"
#include "errmacros.h"

/**
 * SELECT vs POLL vs EPOLL
 * - select gaat door lijst van fileds 1,2,...,200 tot hij connectie vind (niet performant, O(fd_id)) 
 * - poll houdt open fd bij dus rechtstreeks naar open connectie (nog steeds O(#connections))
 * - epoll is 0(1) bij edge trigger maar in mijn geval gebruik ik level trigger dus zelfde als poll
 * epoll gebruiken zou meer problemen opleveren zonder performance gain
 * CONCLUSIE: poll gebruiken
*/

/* global variables */
dplist_t * sockets;

void socket_free(void ** element){
    tcp_close(&((tcp_dpl_t *) *element)->socket);
    free(*element);
    *element = NULL;
}

void * socket_copy(void * element){
    tcp_dpl_t * copy = (tcp_dpl_t *) malloc(sizeof(tcp_dpl_t));
    copy->id = ((tcp_dpl_t *)element)->id;
    copy->socket = ((tcp_dpl_t *)element)->socket;
    copy->last_active = ((tcp_dpl_t*)element)->last_active;
    return copy;
}

int socket_compare(void * el1, void * el2){
    return 0;
}


void connmgr_listen(int port_nr, sbuffer_t * sbuffer){
    sockets = dpl_create(socket_copy, socket_free, socket_compare);

    tcpsock_t * tcp_server;
    tcp_dpl_t * client = malloc(sizeof(tcp_dpl_t));
    ALLOCFAILURE(client);
    pollfd_t * poll_fds = malloc(sizeof(pollfd_t));
    ALLOCFAILURE(poll_fds);
    
    if (tcp_passive_open(&tcp_server, port_nr) != TCP_NO_ERROR) exit(EXIT_FAILURE);

    tcp_get_sd(tcp_server, &(poll_fds[0].fd));
    poll_fds[0].events = POLLIN;
    sensor_ts_t time_diff;
    int conn_count = 0;
    bool new_connection = false;

    char * send_buf;
    asprintf(&send_buf, "Listening for incoming connections on port %i", port_nr);
    write_to_fifo(send_buf);
    free(send_buf);
    do{
        int poll_result;
        poll_result = poll(poll_fds, conn_count +1,TIMEOUT*1000);
        if(poll_result>0){
            if(poll_fds[0].revents & POLLIN){
                tcp_wait_for_connection(tcp_server, &(client->socket));
                
                poll_fds = (pollfd_t *) realloc(poll_fds, sizeof(pollfd_t)*(conn_count+2));
                ALLOCFAILURE(poll_fds);
                tcp_get_sd(client->socket, &(poll_fds[conn_count+1].fd));
                poll_fds[conn_count+1].events = POLLIN;
                poll_fds[conn_count+1].revents = 0;
                client->last_active = (sensor_ts_t) time(NULL);
                dpl_insert_at_index(sockets, client, conn_count+1, true);
                conn_count++;
                new_connection = true;
            }
        }
        for (int i = 0; i < conn_count; i++){
            tcp_dpl_t * curr_client = dpl_get_element_at_index(sockets, i);
            if(poll_fds[i+1].revents & POLLIN){
                sensor_data_t_packed * data = malloc(sizeof(sensor_data_t_packed));
                int bytes, tcp_result = 0;

                // read sensor ID
                bytes = sizeof(data->id);
                tcp_result += tcp_receive(curr_client->socket, (void *) &data->id, &bytes);
                // read temperature
                bytes = sizeof(data->value);
                tcp_result += tcp_receive(curr_client->socket, (void *) &data->value, &bytes);
                // read timestamp
                bytes = sizeof(data->ts);
                tcp_result += tcp_receive(curr_client->socket, (void *) &data->ts, &bytes);
                if ((tcp_result == TCP_NO_ERROR) && bytes) {
                    sbuffer_insert(sbuffer, data);
                    curr_client->last_active = (sensor_ts_t) time(NULL); //update last_active only if new data is read
                    /** LOG new connection **/
                    if(i == conn_count-1 && new_connection){
                        curr_client->id = data->id;
                        asprintf(&send_buf, "A sensor node with id %i has opened a new connection", data->id);
                        write_to_fifo(send_buf);
                        free(send_buf);
                        new_connection = false;
                    }
                }
                free(data);
            }
            time_diff = time(NULL) - curr_client->last_active;
            if(time_diff > TIMEOUT){
                for (int y = i +1; y < conn_count -1; y++) poll_fds[y] = poll_fds[y+1]; //shift all items in poll_fds to remove hole
                poll_fds = (pollfd_t *) realloc(poll_fds, sizeof(pollfd_t)*(conn_count)); //=conn_count +1 -1
                ALLOCFAILURE(poll_fds);

                int removal_id = ((tcp_dpl_t *)dpl_get_element_at_index(sockets, i))->id;
                asprintf(&send_buf, "The sensor node with id %i has closed the connection", removal_id);
                write_to_fifo(send_buf);
                free(send_buf);
                dpl_remove_at_index(sockets,i,true);
                conn_count--;
            }
        }
    }while(conn_count > 0);
    free(poll_fds);
    free(client);
    tcp_close(&tcp_server);
}

void connmgr_free(){
    dpl_free(&sockets, true);
}
