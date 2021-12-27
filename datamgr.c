/**
 * \author Samuel Tocci
 */

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
        sensor_t * sensor = malloc(sizeof(sensor_t));
        sensor->sensor_id = curr_sensor;
        sensor->room_id = curr_room;
        for (int i = 0; i < RUN_AVG_LENGTH ; i++){
            sensor->run_value[i] = 0;
        }
        dpl_insert_at_index(sensor_list, sensor,-1,true); //initialize sensor in sensorlist
    }

    sensor_data_t_packed * sensor_data;
    while ((sensor_data = sbuffer_head(sbuffer)) != NULL){
        sensor_data_t_packed *dummy_data = malloc(sizeof(sensor_data_t_packed));
        dummy_data->id = sensor_data->id;

        int index = dpl_get_index_of_element(sensor_list, dummy_data); //search element with same id in sensor_list
        printf("%i\n",index );
        free(dummy_data);
        sensor_t * element; // element staat al in heap door sensor map loop
        element = dpl_get_element_at_index(sensor_list, index);
        if(element == NULL)printf("amen\n");
        printf("amen before\n");
        element->ts = sensor_data->ts;
        printf("amen after\n");

        for (int i = 0; i < RUN_AVG_LENGTH; i++){ //find empty spot in run_value[]
            if(element->run_value[RUN_AVG_LENGTH-1] != 0){
                sensor_value_t avg = datamgr_get_avg(element->sensor_id);
                if (avg <  SET_MIN_TEMP){ 
                    fprintf(stderr,"under min: %f\n", avg);
                }
                if (avg > SET_MAX_TEMP){ 
                    fprintf(stderr,"over max: %f\n",avg);
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
    int index = dpl_get_index_of_element(sensor_list, dummy_data);
    free(dummy_data);

    sensor_t * element = dpl_get_element_at_index(sensor_list, index);
    return element;
}

