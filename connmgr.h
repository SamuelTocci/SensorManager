/**
 * \author Samuel Tocci
 */

#ifndef CONNMGR_H_
#define CONNMGR_H_

#include "sbuffer.h"

/**
 * polls all connections and puts data in file
*/
void connmgr_listen(int port_number, sbuffer_t * sbuffer);


/**
 * free socket descriptor dpl 
 */
void connmgr_free();

#endif  //CONNMGR_H_