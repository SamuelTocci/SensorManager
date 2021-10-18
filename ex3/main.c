#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define FREQUENCY 5
#define TEMP_MIN 5
#define TEMP_MAX 25

void main(void){
	srand(time(0));
	while(1){
		printf("Temperature = %d @", TEMP_MIN + rand()%(TEMP_MAX - TEMP_MIN +1));
		fflush(stdout);
		system("date");
		sleep(FREQUENCY);
	}
}
