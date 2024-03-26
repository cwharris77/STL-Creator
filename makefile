gcc_args = gcc -Wall -Werror -std=c11 -g

generator: generator.c 3d.o
	${gcc_args} generator.c -o generator 3d.o
3d.o: 3d.c
	${gcc_args} -c 3d.c -o 3d.o
clean:
	rm -f generator 3d.o
clean_stl:
	rm -f *.stl
