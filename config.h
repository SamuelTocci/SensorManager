/**
 * \author Samuel Tocci
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifndef DB_NAME
#define DB_NAME Sensor.db
#endif

#ifndef TABLE_NAME
#define TABLE_NAME SensorData
#endif

#define DBCONN sqlite3

#define REAL_TO_STRING(s) #s
#define TO_STRING(s) REAL_TO_STRING(s)

#include <stdint.h>
#include <time.h>

typedef uint16_t sensor_id_t;
typedef double sensor_value_t;
typedef time_t sensor_ts_t;         // UTC timestamp as returned by time() - notice that the size of time_t is different on 32/64 bit machine

/**
 * structure to hold sensor data
 */
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

void write_to_fifo(const char * send_buf);
void read_from_fifo();


#endif /* _CONFIG_H_ */
