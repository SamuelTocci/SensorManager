HEADERS=sbuffer.h config.h
SOURCE=sbuffer.c main.c
EXE=build/sbuffer_test

FLAGS = -g -std=c11 -Werror -pthread -lm $(shell pkg-config --cflags --libs check)

test:
	@echo -e '\n*******************************'
	@echo -e '*** Compiling for UNIT TEST ***'
	@echo -e '*******************************'
	mkdir -p build
	gcc $(SOURCE) -o $(EXE) $(FLAGS)
	@echo -e '\n*************************'
	@echo -e '*** Running UNIT TEST ***'
	@echo -e '*************************'
	./$(EXE)

val:
	mkdir -p build
	gcc $(SOURCE) -o $(EXE) $(FLAGS)
	CK_FORK=no valgrind --leak-check=full $(EXE)

gdb:
	mkdir -p build
	gcc $(SOURCE) -o $(EXE) $(FLAGS)
	CK_FORK=no gdb $(EXE)
