/**
 * \author Samuel Tocci
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <stdint.h>
#include <time.h>
#include <sys/poll.h>
#include "lib/tcpsock.h"

#ifndef RUN_AVG_LENGTH
#define RUN_AVG_LENGTH 5
#endif

#ifndef SET_MAX_TEMP
#endif

#ifndef SET_MIN_TEMP
#endif

#ifndef DB_NAME
#define DB_NAME Sensor.db
#endif

#ifndef TABLE_NAME
#define TABLE_NAME SensorData
#endif

#ifndef TIMEOUT
#endif

#define REAL_TO_STRING(s) #s
#define TO_STRING(s) REAL_TO_STRING(s)

#define FIFO_NAME "LOGFIFO"

#define DBCONN sqlite3

typedef uint16_t sensor_id_t;
typedef uint16_t room_id_t;
typedef double sensor_value_t;
typedef time_t sensor_ts_t;         // UTC timestamp as returned by time() - notice that the size of time_t is different on 32/64 bit machine

typedef struct {
    sensor_id_t id;
    sensor_value_t value;
    sensor_ts_t ts;
} sensor_data_t;

typedef struct{
    sensor_id_t id __attribute__((packed));
    sensor_value_t value __attribute__((packed));
    sensor_ts_t ts __attribute__((packed));
}sensor_data_t_packed;

typedef struct pollfd pollfd_t;

typedef struct tcpsock tcpsock_t;

struct tcpsock {
    long cookie;        /**< if the socket is bound, cookie should be equal to MAGIC_COOKIE */
    // remark: the use of magic cookies doesn't guarantee a 'bullet proof' test
    int sd;             /**< socket descriptor */
    char *ip_addr;      /**< socket IP address */
    int port;           /**< socket port number */
};

typedef struct{
    sensor_ts_t last_active;
    tcpsock_t * socket;
    sensor_id_t id;
}tcp_dpl_t;


void write_to_fifo(const char * send_buf);
void read_from_fifo();

#endif /* _CONFIG_H_ */