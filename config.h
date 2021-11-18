/**
 * \author Samuel Tocci
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>
#include <time.h>

typedef uint16_t sensor_id_t;
typedef uint16_t room_id_t;
typedef double sensor_value_t;
typedef time_t sensor_ts_t;         // UTC timestamp as returned by time() - notice that the size of time_t is different on 32/64 bit machine
/**
 * Sensor data is defined as a struct with the following fields:
 *    •sensor_id: a positive 16-bit integer;
 *    •temperature: a double;
 *    •timestamp: a time_t value; 
 * 
 * */
typedef struct {
    sensor_id_t id;         /** < sensor id */
    sensor_value_t value;   /** < sensor value */
    sensor_ts_t ts;         /** < sensor timestamp */
} sensor_data_t;

typedef struct{
    sensor_id_t id __attribute__((packed));
    sensor_value_t value __attribute__((packed));
    sensor_ts_t ts __attribute__((packed));
}sensor_data_t_packed;

void* element_copy(void * element);
void element_free(void ** element);
int element_compare(void * x, void * y);


#endif /* _CONFIG_H_ */
