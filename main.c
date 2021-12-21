#include "connmgr.h"
#include <stdio.h>
#include <stdlib.h>
#include "config.h"

int main(void){
    connmgr_listen(PORT);
    connmgr_free();
    return 0;
}