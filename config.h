/**
 * \author Samuel Tocci
 */

#define _GNU_SOURCE

#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifndef OUTPUT_NAME
#define OUTPUT_NAME "sensor_data_recv.txt"
#endif

#include <stdint.h>
#include <time.h>
#include <sys/poll.h>
#include "lib/tcpsock.h"

typedef uint16_t sensor_id_t;
typedef double sensor_value_t;
typedef time_t sensor_ts_t;         // UTC timestamp as returned by time() - notice that the size of time_t is different on 32/64 bit machine

typedef struct {
    sensor_id_t id;
    sensor_value_t value;
    sensor_ts_t ts;
} sensor_data_t;

typedef struct pollfd pollfd_t;

typedef struct{
    pollfd_t poll_fd;
    tcpsock_t * socket;
} poll_t;




#endif /* _CONFIG_H_ */
