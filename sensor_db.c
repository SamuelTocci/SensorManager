/**
 * \author Samuel Tocci
 */

#define _GNU_SOURCE

#ifndef _SENSOR_DB_H_
#define _SENSOR_DB_H_

#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#include <sqlite3.h>
#include "sensor_db.h"

typedef int (*callback_t)(void *, int, char **, char **);

DBCONN *init_connection(char clear_up_flag){
	sqlite3 *conn;
	char *err_msg = 0;
	char *query;
	
	int pipefd[2];
	char buff;
	if(pipe(pipefd) == -1){
		exit(EXIT_FAILURE);
	}

	int pid = fork();

	if(pid == 0){//child process
		close(pipefd[1]); //close unused write end of pipe
		while (read(pipefd[0], &buff, 1) > 0){
			/* code */
		}
		
		log_mgr(&pipefd[0]);
	} else { //parent process
		close(pipefd[0]); //close unused read end of pipe

		int rc = sqlite3_open(TO_STRING(DB_NAME), &conn);
		if (rc != SQLITE_OK){
			//fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(conn));
			write(pipefd[1],sqlite3_errmsg(conn), sizeof(sqlite3_errmsg(conn)));
			sqlite3_close(conn);
			free(query);
			return NULL;
		}
		if(clear_up_flag == 1){
			asprintf(&query, "DROP TABLE IF EXISTS %1$s;"
							"CREATE TABLE %1$s(Id Integer PRIMARY KEY AUTOINCREMENT, sensor_id Integer, sensor_value DECIMAL(4,2), timestamp TIMESTAMP);", TO_STRING(TABLE_NAME));
			sqlite3_exec(conn, query, 0,0, &err_msg);
			free(query);
		}
	}
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

void log_mgr(void ** pipeReadEnd){
	printf("Log: %s\n", pipeReadEnd);
}

#endif /* _SENSOR_DB_H_ */
