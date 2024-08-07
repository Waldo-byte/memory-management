

LD_SRC = malloc.c
LD_LIB = malloc.a

SRCS = main.c
BIN = main


all: compile-lib compile-bin
	@echo "Running Make all"

compile-lib:
	@echo "Compiling lib"
	gcc -o ${LD_LIB} ${LD_SRC} -c

compile-bin:
	@echo "Compiling bin"
	gcc -o ${BIN} ${SRCS} ${LD_LIB}

clean:
	@rm -rf *.a ${BIN}
