# Create your own targets that compile the application
seq:
	gcc -g -o server -lpthread test_server.c lib/tcpsock.c
	gcc -g -o client -lpthread sensor_node.c lib/tcpsock.c 

par:
	gcc -g -o server -lpthread main.c connmgr.c lib/tcpsock.c lib/dplist.c
	gcc -g -o client -lpthread sensor_node.c lib/tcpsock.c
	./server

cl:
	./client 15 2 127.0.0.1 5678


gdb:
	gcc -g -o server -lpthread main.c connmgr.c lib/tcpsock.c lib/dplist.c
	gcc -g -o client -lpthread sensor_node.c lib/tcpsock.c
	CK_FORK=no gdb server

val:
	gcc -g -o server -lpthread main.c connmgr.c lib/tcpsock.c lib/dplist.c
	gcc -g -o client -lpthread sensor_node.c lib/tcpsock.c
	CK_FORK=no valgrind --leak-check=full ./server

# the files for ex2 will be ziped and are then ready to
# be submitted to labtools.groept.be
zip:
	zip lab7.zip connmgr.c connmgr.h config.h lib/dplist.c lib/dplist.h lib/tcpsock.c lib/tcpsock.h
