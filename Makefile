CC=g++
EXE=testExec

SRC=main.cpp
OBJ=main.o

CFLAGS := $(shell sdl2-config --cflags)
CFLAGS += -Wall -g
LDFLAGS := $(shell sdl2-config --libs)

# Compile source file into object file
${OBJ}: main.cpp
	${CC} ${CFLAGS} -c ${SRC} -o ${OBJ}

# Link object file to create executable
${EXE}: ${OBJ}
	${CC} -o ${EXE} ${OBJ} ${LDFLAGS} 

# Default target (only builds)
all: ${EXE}

# Run the executable
run: ${EXE}
	./${EXE}

# Clean the build files
clean:
	rm -f ${OBJ} ${EXE}
