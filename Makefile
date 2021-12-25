# Create your own targets that compile the application
HEADER=sensor_db.h
SOURCE=sensor_db.c lib/dplist.c data_mgr.c
FLAGS= -Wall -g -std=c11 -Werror -lm $(shell pkg-config --cflags --libs check sqlite3)
EXE=./build/storage_mgr

test:
	mkdir -p ./build
	gcc ./sensor_db.c main.c -o ./build/storage_mgr $(FLAGS)
	./build/storage_mgr

val:
	mkdir -p ./build
	gcc ./sensor_db.c main.c -o ./build/storage_mgr $(FLAGS)
	CK_FORK=no valgrind --leak-check=full --show-leak-kinds=all $(EXE)


# the files for ex2 will be ziped and are then ready to
# be submitted to labtools.groept.be
zip:
	zip lab6.zip sensor_db.c config.h errmacros.h
