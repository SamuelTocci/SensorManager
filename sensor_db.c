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

#define FIFO_NAME "LOGFIFO"
#define MAXBUFF 80

typedef int (*callback_t)(void *, int, char **, char **);

DBCONN *init_connection(char clear_up_flag){
	sqlite3 *conn;
	char *err_msg = 0;
	char *query;
	int rc = sqlite3_open(TO_STRING(DB_NAME), &conn);
	if (rc != SQLITE_OK){
		const char * send_buf = sqlite3_errmsg(conn);
		write_to_fifo(send_buf);
		
		sqlite3_close(conn);
		free(query);
		return NULL;
	}
	if(clear_up_flag == 1){
		asprintf(&query, "DROP TABLE IF EXISTS %1$s;"
						"CREATE TABLE %1$s(Id Integer PRIMARY KEY AUTOINCREMENT, sensor_id Integer, sensor_value DECIMAL(4,2), timestamp TIMESTAMP);", TO_STRING(TABLE_NAME));
	} else {
		asprintf(&query,"CREATE TABLE %1$s(Id Integer PRIMARY KEY AUTOINCREMENT, sensor_id Integer, sensor_value DECIMAL(4,2), timestamp TIMESTAMP);", TO_STRING(TABLE_NAME));
	}

	int result = sqlite3_exec(conn, query, 0,0, &err_msg);
	if(result == SQLITE_OK){
		const char * send_buf = "Succesfully created Table";
		write_to_fifo(send_buf);
	} else {
		const char * send_buf = sqlite3_errmsg(conn);
		write_to_fifo(send_buf);
	}
	free(query);
	return conn;
}

void disconnect(DBCONN *conn){
	sqlite3_close(conn);
}

int insert_sensor(DBCONN *conn, sensor_id_t id, sensor_value_t value, sensor_ts_t ts){
	char *query;
	char *err_msg = 0;

	asprintf(&query, "INSERT INTO %1$s (sensor_id, sensor_value, timestamp) VALUES(%2$u, %3$f, %4$ld);" //0 for id row -> auto assign id with increment
						,TO_STRING(TABLE_NAME), id, value, ts);
	int result = sqlite3_exec(conn, query, 0,0, &err_msg);
	free(query);
	if (result != SQLITE_OK){
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(conn));
		return 1;
	}
	return 0;
}

int insert_sensor_from_file(DBCONN *conn, FILE *sensor_data);//TODO

int find_sensor_all(DBCONN *conn, callback_t f){
	char *query;
	char *err_msg = 0;

	asprintf(&query, "SELECT * FROM %1s", TO_STRING(TABLE_NAME));
	int result = sqlite3_exec(conn, query, f,0, &err_msg);
	free(query);
	if (result != SQLITE_OK){
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(conn));
		return 1;
	}
	return 0;
}

int find_sensor_by_value(DBCONN *conn, sensor_value_t value, callback_t f){
	char *query;
	char *err_msg = 0;

	asprintf(&query, "SELECT * FROM %1s WHERE sensor_value = %2f", TO_STRING(TABLE_NAME), value);
	int result = sqlite3_exec(conn, query, f,0, &err_msg);
	free(query);
	if (result != SQLITE_OK){
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(conn));
		return 1;
	}
	return 0;
}

int find_sensor_exceed_value(DBCONN *conn, sensor_value_t value, callback_t f){
	char *query;
	char *err_msg = 0;

	asprintf(&query, "SELECT * FROM %1s WHERE sensor_value > %2f", TO_STRING(TABLE_NAME), value);
	int result = sqlite3_exec(conn, query, f,0, &err_msg);
	free(query);
	if (result != SQLITE_OK){
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(conn));
		return 1;
	}
	return 0;
}

int find_sensor_by_timestamp(DBCONN *conn, sensor_ts_t ts, callback_t f){
	char *query;
	char *err_msg = 0;

	asprintf(&query, "SELECT * FROM %1s WHERE timestamp = %2ld", TO_STRING(TABLE_NAME), ts);
	int result = sqlite3_exec(conn, query, f,0, &err_msg);
	free(query);
	if (result != SQLITE_OK){
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(conn));
		return 1;
	}
	return 0;
}

int find_sensor_after_timestamp(DBCONN *conn, sensor_ts_t ts, callback_t f){
	char *query;
	char *err_msg = 0;

	asprintf(&query, "SELECT * FROM %1s WHERE timestamp > %2ld", TO_STRING(TABLE_NAME), ts);
	int result = sqlite3_exec(conn, query, f,0, &err_msg);
	free(query);
	if (result != SQLITE_OK){
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(conn));
		return 1;
	}
	return 0;
}

void write_to_fifo(const char * send_buf){
	FILE *fptr;
	int result;

	/* Create the FIFO if it does not exist */ 
	result = mkfifo(FIFO_NAME, 0666);
	CHECK_MKFIFO(result); 

	fptr = fopen(FIFO_NAME, "w"); 
	printf("syncing with reader ok\n");
	FILE_OPEN_ERROR(fptr);

	if ( fputs( send_buf, fptr ) == EOF )
	{
		fprintf( stderr, "Error writing data to fifo\n");
		exit( EXIT_FAILURE );
	} 
	FFLUSH_ERROR(fflush(fptr));
	printf("Message send: %s \n", send_buf); 
	sleep(1);

	result = fclose( fptr );
	FILE_CLOSE_ERROR(result);
}

void read_from_fifo(){
	int result;
	char *str_result;
	char recv_buf[MAXBUFF]; 
	FILE *fptr;
	/* Create the FIFO if it does not exist */ 
	result = mkfifo(FIFO_NAME, 0666);
	CHECK_MKFIFO(result); 
	
	fptr = fopen(FIFO_NAME, "r"); 
	printf("syncing with writer ok\n");
	FILE_OPEN_ERROR(fptr);

	do 
	{
		str_result = fgets(recv_buf, MAXBUFF, fptr);
		if ( str_result != NULL )
		{ 
			printf("LOG: %s \n", recv_buf); 
		}
	} while ( str_result != NULL ); 

	result = fclose( fptr );
	FILE_CLOSE_ERROR(result);
}
