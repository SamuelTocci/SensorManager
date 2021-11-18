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

dplist_t * sensor_list;


/**
 *  This method holds the core functionality of your datamgr. It takes in 2 file pointers to the sensor files and parses them. 
 *  When the method finishes all data should be in the internal pointer list and all log messages should be printed to stderr.
 *  \param fp_sensor_map file pointer to the map file
 *  \param fp_sensor_data file pointer to the binary data file
 */
void datamgr_parse_sensor_files(FILE *fp_sensor_map, FILE *fp_sensor_data){
    room_id_t curr_room;
    sensor_id_t curr_sensor;
    //TODO: add catch if files are not opened correctly
    sensor_list = dpl_create(element_copy, element_free, element_compare);
    while (fscanf(fp_sensor_map, "%hd %hd", &curr_room, &curr_sensor)>0){
        sensor_t * sensor;
        sensor = malloc(sizeof(sensor_t));
        sensor->sensor_id = curr_sensor;
        sensor->room_id = curr_room;
        for (int i = 0; i < RUN_AVG_LENGTH ; i++){
            sensor->run_value[i] = 0;
        }
        dpl_insert_at_index(sensor_list, sensor,-1,false); //initialize sensor in sensorlist
    }

    sensor_data_t_packed sensor_data;
    while (fread(&sensor_data, sizeof(sensor_data_t_packed),1,fp_sensor_data)>0){
        //printf ("id = %d value = %f %ld\n", sensor_data.id, sensor_data.value, sensor_data.ts);
        //TODO: make it output to dplist instead of printing (sensor_list)
        sensor_data_t_packed *dummy_data = malloc(sizeof(sensor_data_t_packed));
        dummy_data->id = sensor_data.id;

        int index = dpl_get_index_of_element(sensor_list, dummy_data); //search element with same id in sensor_list
        free(dummy_data);
        sensor_t * element = malloc(sizeof(sensor_t));
        element = dpl_get_element_at_index(sensor_list, index);

        element->ts = sensor_data.ts;
        for (int i = 0; i < RUN_AVG_LENGTH-1; i++){
            if(element->run_value[i] == 0){ 
                element->run_value[i] = sensor_data.value;
                break;
            }
            if(i == RUN_AVG_LENGTH-1){
                for (int j = 0; j < RUN_AVG_LENGTH-1; i++){
                    element->run_value[j] = 0;
                }
                element->run_value[0] = sensor_data.value; 
            }
        }
        //printf ("id = %d value = %f %ld\n", element->sensor_id, element->run_value[0], element->ts);

    }

    // for (int i = 0; i < datamgr_get_total_sensors(sensor_list) ; i++){
    //     sensor_t * element = dpl_get_element_at_index(sensor_list, i);
    //     double avg = 0;
    //     for (int j = 0; j < RUN_AVG_LENGTH ; j++){
    //         avg += element->run_value[j];
    //     }
    //     avg = avg/RUN_AVG_LENGTH;
        
    //     printf ("id = %d value = %f %ld\n", element->sensor_id, avg, element->ts);
    // }
    
}

/**
 * This method should be called to clean up the datamgr, and to free all used memory. 
 * After this, any call to datamgr_get_room_id, datamgr_get_avg, datamgr_get_last_modified or datamgr_get_total_sensors will not return a valid result
 */
void datamgr_free(){
    dpl_free(&sensor_list,true);
}

/**
 * Gets the room ID for a certain sensor ID
 * Use ERROR_HANDLER() if sensor_id is invalid
 * \param sensor_id the sensor id to look for
 * \return the corresponding room id
 */
uint16_t datamgr_get_room_id(sensor_id_t sensor_id){
    return 0;
}

/**
 * Gets the running AVG of a certain senor ID (if less then RUN_AVG_LENGTH measurements are recorded the avg is 0)
 * Use ERROR_HANDLER() if sensor_id is invalid
 * \param sensor_id the sensor id to look for
 * \return the running AVG of the given sensor
 */
sensor_value_t datamgr_get_avg(sensor_id_t sensor_id){
    return 0;
}

/**
 * Returns the time of the last reading for a certain sensor ID
 * Use ERROR_HANDLER() if sensor_id is invalid
 * \param sensor_id the sensor id to look for
 * \return the last modified timestamp for the given sensor
 */
time_t datamgr_get_last_modified(sensor_id_t sensor_id){
    return 0;
}

/**
 *  Return the total amount of unique sensor ID's recorded by the datamgr
 *  \return the total amount of sensors
 */
int datamgr_get_total_sensors(){
    return dpl_size(sensor_list);
}

