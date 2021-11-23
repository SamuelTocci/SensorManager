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

/**
 * Make a connection to the database server
 * Create (open) a database with name DB_NAME having 1 table named TABLE_NAME  
 * \param clear_up_flag if the table existed, clear up the existing data when clear_up_flag is set to 1
 * \return the connection for success, NULL if an error occurs
 */
DBCONN *init_connection(char clear_up_flag){
	sqlite3 *conn;
	char *err_msg = 0;
	char *query;

	int rc = sqlite3_open(TO_STRING(DB_NAME), &conn);
	if (rc != SQLITE_OK){
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(conn));
		sqlite3_close(conn);
		free(query);
		return NULL;
	}
	if(clear_up_flag == 1){
		asprintf(&query, "DROP TABLE IF EXISTS %1$s;"
						"CREATE TABLE %1$s(Id Integer PRIMARY KEY AUTOINCREMENT, sensor_id Integer, sensor_value DECIMAL(4,2), timestamp TIMESTAMP);", TO_STRING(TABLE_NAME));
		int result = sqlite3_exec(conn, query, 0,0, &err_msg);
		free(query);
	}
	return conn;
}

/**
 * Disconnect from the database server
 * \param conn pointer to the current connection
 */
void disconnect(DBCONN *conn){
	sqlite3_close(conn);
}

/**
 * Write an INSERT query to insert a single sensor measurement
 * \param conn pointer to the current connection
 * \param id the sensor id
 * \param value the measurement value
 * \param ts the measurement timestamp
 * \return zero for success, and non-zero if an error occurs
 */
int insert_sensor(DBCONN *conn, sensor_id_t id, sensor_value_t value, sensor_ts_t ts){
	char *query;
	char *err_msg = 0;

	asprintf(&query, "INSERT INTO %1$s (sensor_id, sensor_value, timestamp) VALUES(%2$u, %3$f, %4$ld);" //0 for id row -> auto assign id with increment
						,TO_STRING(TABLE_NAME), id, value, ts);
	int result = sqlite3_exec(conn, query, 0,0, &err_msg);
	free(query);
	if (result == SQLITE_OK){
		fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(conn));
		sqlite3_close(conn);
		return 1;
	}
	return 0;
		// INSERT INTO COMPANY (NAME,AGE,ADDRESS,SALARY)
		// VALUES ( 'Paul', 32, 'California', 20000.00 );

}

/**
 * Write an INSERT query to insert all sensor measurements available in the file 'sensor_data'
 * \param conn pointer to the current connection
 * \param sensor_data a file pointer to binary file containing sensor data
 * \return zero for success, and non-zero if an error occurs
 */
int insert_sensor_from_file(DBCONN *conn, FILE *sensor_data);

/**
  * Write a SELECT query to select all sensor measurements in the table 
  * The callback function is applied to every row in the result
  * \param conn pointer to the current connection
  * \param f function pointer to the callback method that will handle the result set
  * \return zero for success, and non-zero if an error occurs
  */
int find_sensor_all(DBCONN *conn, callback_t f);

/**
 * Write a SELECT query to return all sensor measurements having a temperature of 'value'
 * The callback function is applied to every row in the result
 * \param conn pointer to the current connection
 * \param value the value to be queried
 * \param f function pointer to the callback method that will handle the result set
 * \return zero for success, and non-zero if an error occurs
 */
int find_sensor_by_value(DBCONN *conn, sensor_value_t value, callback_t f);

/**
 * Write a SELECT query to return all sensor measurements of which the temperature exceeds 'value'
 * The callback function is applied to every row in the result
 * \param conn pointer to the current connection
 * \param value the value to be queried
 * \param f function pointer to the callback method that will handle the result set
 * \return zero for success, and non-zero if an error occurs
 */
int find_sensor_exceed_value(DBCONN *conn, sensor_value_t value, callback_t f);

/**
 * Write a SELECT query to return all sensor measurements having a timestamp 'ts'
 * The callback function is applied to every row in the result
 * \param conn pointer to the current connection
 * \param ts the timestamp to be queried
 * \param f function pointer to the callback method that will handle the result set
 * \return zero for success, and non-zero if an error occurs
 */
int find_sensor_by_timestamp(DBCONN *conn, sensor_ts_t ts, callback_t f);

/**
 * Write a SELECT query to return all sensor measurements recorded after timestamp 'ts'
 * The callback function is applied to every row in the result
 * \param conn pointer to the current connection
 * \param ts the timestamp to be queried
 * \param f function pointer to the callback method that will handle the result set
 * \return zero for success, and non-zero if an error occurs
 */
int find_sensor_after_timestamp(DBCONN *conn, sensor_ts_t ts, callback_t f);

#endif /* _SENSOR_DB_H_ */
