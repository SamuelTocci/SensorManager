# if you created a subfolder for every exercise this target will
# build and execute the main.c file in the ex3 folder
ex1: ex1/main.c
	mkdir -p build
	gcc -g -Wall -Werror -o ./build/ex1 ex1/main.c ex1/ma_malloc.c
	./build/ex1

# the main file in the ex3 folder will be ziped and is then ready to
# be submitted to labtools.groept.be
zip: ex1/main.c
	cd ex1 && zip ../lab3_ex1.zip main.c