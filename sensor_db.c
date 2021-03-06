/**
 * \author Samuel Tocci
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include <sqlite3.h>
#include <unistd.h>
#include "errmacros.h"
#include "sensor_db.h"
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include "sbuffer.h"

#define MAXBUFF 90

int log_line;

pthread_mutex_t fifo_mutex;
typedef int (*callback_t)(void *, int, char **, char **);

DBCONN *init_connection(char clear_up_flag){
	if(pthread_mutex_init(&fifo_mutex, NULL) != 0) exit(EXIT_FAILURE);
	log_line = 0;

	sqlite3 *conn;
	char *err_msg = 0;
	char *query;

	/* Establish connection to db */
	int result = sqlite3_open(TO_STRING(DB_NAME), &conn);
	if (result != SQLITE_OK){
		const char * send_buf = "Unable to connect to SQL server";
		write_to_fifo(send_buf);
		sqlite3_close(conn);
		exit(EXIT_FAILURE);
	} else {
		const char * send_buf = "Connection to SQL server established";
		write_to_fifo(send_buf);
	}
	/* Creating (and clearing) Table */
	if(clear_up_flag == 1){
		asprintf(&query, "DROP TABLE IF EXISTS %1$s;"
						"CREATE TABLE %1$s(Id Integer PRIMARY KEY AUTOINCREMENT, sensor_id Integer, sensor_value DECIMAL(4,2), timestamp TIMESTAMP);", TO_STRING(TABLE_NAME));
	} else {
		asprintf(&query,"CREATE TABLE %1$s(Id Integer PRIMARY KEY AUTOINCREMENT, sensor_id Integer, sensor_value DECIMAL(4,2), timestamp TIMESTAMP);", TO_STRING(TABLE_NAME));
	}
	result = sqlite3_exec(conn, query, 0,0, &err_msg);
	if(result != SQLITE_OK){
		const char * send_buf = sqlite3_errmsg(conn);
		write_to_fifo(send_buf);
	} else {
		char * send_buf;
		asprintf(&send_buf, "New table %s created", TO_STRING(TABLE_NAME));
		write_to_fifo(send_buf);
		free(send_buf);
	}
	free(query);
	return conn;
}

void disconnect(DBCONN *conn){
	sqlite3_close(conn);
	const char * send_buf = "Connection to SQL server lost";
	write_to_fifo(send_buf);
}

int insert_sensor(DBCONN *conn, sensor_id_t id, sensor_value_t value, sensor_ts_t ts){
	char *query;
	char *err_msg = 0;

	asprintf(&query, "INSERT INTO %1$s (sensor_id, sensor_value, timestamp) VALUES(%2$u, %3$f, %4$ld);" //0 for id row -> auto assign id with increment
						,TO_STRING(TABLE_NAME), id, value, ts);
	int result = sqlite3_exec(conn, query, 0,0, &err_msg);
	free(query);
	if (result != SQLITE_OK){
		const char * send_buf = sqlite3_errmsg(conn);
		write_to_fifo(send_buf);
		return DB_ERROR;
	} else {
		const char * send_buf = "New SQL data entry";
		write_to_fifo(send_buf);
	}
	return DB_SUCCESS;
}

int insert_sensor_from_file(DBCONN *conn, sbuffer_t * sbuffer){
	sensor_data_t_packed * sensor_data;
	time_t latest = time(NULL);
	do{
		while ((sensor_data = sbuffer_next(sbuffer,1)) != NULL){
			int result = insert_sensor(conn, sensor_data->id, sensor_data->value, sensor_data->ts);
			latest = time(NULL);
			if(result != 0)return result;
		}
	} while (time(NULL) - latest < 3*TIMEOUT);

	free(sensor_data);
	return DB_SUCCESS;
}

void write_to_fifo(const char * send_buf){
	FILE *fptr;
	int result;

	/* Create the FIFO if it does not exist */ 
	result = mkfifo(FIFO_NAME, 0666);
	CHECK_MKFIFO(result); 

	pthread_mutex_lock(&fifo_mutex);
	fptr = fopen(FIFO_NAME, "w"); 
	FILE_OPEN_ERROR(fptr);

	if ( fputs( send_buf, fptr ) == EOF )
	{
		fprintf( stderr, "Error writing data to fifo\n");
		exit( EXIT_FAILURE );
	} 
	FFLUSH_ERROR(fflush(fptr));

	result = fclose( fptr );
	FILE_CLOSE_ERROR(result);
	pthread_mutex_unlock(&fifo_mutex);
}

void read_from_fifo(){
	int result;
	char *str_result;
	char recv_buf[MAXBUFF];
	FILE *fptr;

	FILE *log;
	/* Create the FIFO if it does not exist */ 
	result = mkfifo(FIFO_NAME, 0666);
	CHECK_MKFIFO(result); 
	
	fptr = fopen(FIFO_NAME, "r"); 
	FILE_OPEN_ERROR(fptr);

	log = fopen("gateway.log", "a");
	FILE_OPEN_ERROR(log);

	FFLUSH_ERROR(fflush(fptr));
	pthread_mutex_lock(&fifo_mutex);
	do{
		str_result = fgets(recv_buf, MAXBUFF, fptr);
		if (str_result != NULL){
			log_line ++;
			#ifdef DEBUG 
				printf("LOG:[%i] <%li> %s \n", log_line, time(NULL), recv_buf);
			#endif
			fprintf(log,"%i %li %s \n", log_line, time(NULL), recv_buf);
		}
	} while ( str_result != NULL );

	result = fclose( fptr );
	FILE_CLOSE_ERROR(result);

	result = fclose(log);
	FILE_CLOSE_ERROR(result);
	pthread_mutex_unlock(&fifo_mutex);

}