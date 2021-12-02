HEADERS=dplist.h
SOURCE=dplist.c dplist_test.c
EXE=build/dplist_test

FLAGS = -g -std=c11 -Werror -lm $(shell pkg-config --cflags --libs check)

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

valgrind :
	CK_FORK=no valgrind --leak-check=full $(EXE)

gdb :
	CK_FORK=no gdb $(EXE)

.PHONY : clean check

check : $(SOURCE) $(HEADERS)
	@echo "Running cppcheck :"
	cppcheck --enable=all --suppress=missingIncludeSystem --error-exitcode=1 $(SOURCE) $(HEADERS)

clean:
	rm -f *.o $(EXE) *~

zip:
	zip lab4_ex3.zip dplist.c
