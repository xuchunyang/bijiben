CC = gcc
CFLAGS = -Wall `pkg-config --cflags --libs gtk+-3.0 webkit2gtk-3.0`

DEPS = biji-webkit2-editor.h
OBJS = biji-webkit2-editor.o notmain.o

TARGET = notmain

all: ${TARGET}

%.o: %.c ${DEPS}
	gcc -c -o $@ $< ${CFLAGS}

${TARGET}: ${OBJS}
	gcc -o $@ $^ ${CFLAGS}

run:
	./${TARGET}

clean:
	rm -f ${TARGET} ${OBJS}

.PHONY: all clean

