# Create your own targets that build the dynamic library for the list and then compile the application

static:
	gcc -c lib/dplist.c -o libdplist.a
	gcc -static datamgr.c main.c -DSET_MAX_TEMP=20 -DSET_MIN_TEMP=10 libdplist.a -o datamgr
	./datamgr

shared:
	gcc -fPIC -c lib/dplist.c
	gcc --shared -o libdplist.so dplist.o
	gcc -g main.c `pkg-config --cflags --libs check` datamgr.c -L./ -Wl,-rpath=./ -ldplist -Wall -Werror -o datamgr_shared -DSET_MAX_TEMP=20 -DSET_MIN_TEMP=10
	./datamgr_shared
# the files for ex3 will be ziped and are then ready to
# be submitted to labtools.groept.be
zip:
	zip lab5_ex3.zip datamgr.c datamgr.h config.h lib/dplist.c lib/dplist.h
