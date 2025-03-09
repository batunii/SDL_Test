CC=g++
EXE=testExec

SRC=main.cpp

OBJ=main.o

CFLAGS := $(shell sdl2-config --cflags)
CFLAGS += -Wall -g
LDFLAGS := $(shell sdl2-config --libs)

${OBJ}: main.cpp
	${CC} ${CFLAGS} -c ${SRC} -o ${OBJ}
	
${EXE}: ${OBJ}
	${CC} -o ${EXE} ${OBJ} ${LDFLAGS} 

all : ${EXE}
	./${EXE}

clean :
	rm ${OBJ} ${EXE}

