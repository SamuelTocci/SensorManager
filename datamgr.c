/**
 * \author Samuel Tocci
 */

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "stdint.h"
#include "datamgr.h"
#include "config.h"
#include "lib/dplist.h"
#include "sbuffer.h"

void * element_copy(void * element){
    sensor_t* copy = malloc(sizeof (sensor_t));
    copy->room_id = ((sensor_t*)element)->room_id;
    copy->sensor_id = ((sensor_t*)element)->sensor_id;
    copy->ts = ((sensor_t*)element)->ts;
    for (int index = 0; index < RUN_AVG_LENGTH; index++){
        copy->run_value[index] = ((sensor_t*)element)->run_value[index];
    }
    return copy;
}

void element_free(void ** element){
    free(*element);
    *element = NULL;
}

int element_compare(void * x, void * y){
    //-1 if smaller, 0 if equal, 1 if bigger
    return ((((sensor_t*)x)->sensor_id < ((sensor_t*)y)->sensor_id) ? -1 : (((sensor_t*)x)->sensor_id == ((sensor_t*)y)->sensor_id) ? 0 : 1);
}

dplist_t * sensor_list;

void datamgr_parse_sensor_files(FILE *fp_sensor_map, sbuffer_t * sbuffer){
    room_id_t curr_room;
    sensor_id_t curr_sensor;
    //TODO: add catch if files are not opened correctly
    sensor_list = dpl_create(element_copy, element_free, element_compare);
    while (fscanf(fp_sensor_map, "%hd %hd", &curr_room, &curr_sensor)>0){
        sensor_t sensor;
        sensor.sensor_id = curr_sensor;
        sensor.room_id = curr_room;
        for (int i = 0; i < RUN_AVG_LENGTH ; i++){
            sensor.run_value[i] = 0;
        }
        dpl_insert_at_index(sensor_list, &sensor,-1,true); //initialize sensor in sensorlist
    }

    sensor_data_t_packed * sensor_data;
    time_t latest = time(NULL);
    do{
        while ((sensor_data = sbuffer_next(sbuffer, 0)) != NULL){
            sensor_t * element = datamgr_get_sensor_with_id(sensor_data->id);
            if(element == NULL){
                char * send_buf; 
                asprintf(&send_buf, "Received sensor data with invalid sensor node ID %i", sensor_data->id);
                write_to_fifo(send_buf);
                free(send_buf);
            } else {
                element->ts = sensor_data->ts;
                for (int i = 0; i < RUN_AVG_LENGTH; i++){ //find empty spot in run_value[]
                    // printf("run_value: %f\n",element->run_value[i] );
                    if(element->run_value[RUN_AVG_LENGTH-1] != 0){
                        sensor_value_t avg = datamgr_get_avg(element->sensor_id);
                        if (avg <  SET_MIN_TEMP){
                            char * send_buf; 
                            asprintf(&send_buf, "The sensor node with id %i reports it’s too cold (running avgtemperature = %f)", element->sensor_id, avg);
                            write_to_fifo(send_buf);
                        }
                        if (avg > SET_MAX_TEMP){ 
                            char * send_buf; 
                            asprintf(&send_buf, "The sensor node with id %i reports it’s too hot (running avgtemperature = %f)", element->sensor_id, avg);
                            write_to_fifo(send_buf);
                            free(send_buf);
                        }
                        for (int j = 0; j < RUN_AVG_LENGTH; j++){ //empty run_value[]
                            element->run_value[j] = 0;
                        }
                    }
                    if(element->run_value[i] == 0){ 
                        element->run_value[i] = sensor_data->value;
                        break;
                    }
                }
            }
            latest = time(NULL);
        }    
    } while (time(NULL) - latest < TIMEOUT);
}

void datamgr_free(){
    dpl_free(&sensor_list,true);
}

uint16_t datamgr_get_room_id(sensor_id_t sensor_id){
    sensor_t * element = datamgr_get_sensor_with_id(sensor_id);
    return element->room_id;
}

sensor_value_t datamgr_get_avg(sensor_id_t sensor_id){
    sensor_t * element = datamgr_get_sensor_with_id(sensor_id);
    if(element == NULL)return -1;
    if(element->run_value[RUN_AVG_LENGTH -1] == 0)return 0;
    float avg = 0;
    for (int i = 0; i < RUN_AVG_LENGTH ; i++){
        sensor_value_t value = element->run_value[i];
        avg += value;
    }
    avg = avg/RUN_AVG_LENGTH;

    return avg;
}

time_t datamgr_get_last_modified(sensor_id_t sensor_id){
    sensor_t * element = datamgr_get_sensor_with_id(sensor_id);
    return element->ts;
}

int datamgr_get_total_sensors(){
    return dpl_size(sensor_list);
}

void *datamgr_get_sensor_with_id(sensor_id_t sensor_id){
    sensor_data_t_packed *dummy_data = malloc(sizeof(sensor_data_t_packed));
    dummy_data->id = sensor_id;
    int index = dpl_get_index_of_element(sensor_list, dummy_data); //returns -1 if no elements match
    if(index == -1) return NULL;
    free(dummy_data);

    sensor_t * element = dpl_get_element_at_index(sensor_list, index);
    return element;
}

