TARGET=bbble

PREFIX=
CC=$(PREFIX)g++

INC=./inc
SRC=./src

CFLAGS=-O2 -pipe -Wall

SOURCES=$(shell find . -name '*.cpp')
OBJECTS=$(patsubst %.cpp,%.o,$(SOURCES))

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS) -I$(INC)

all: $(OBJECTS)
	$(CC) -o $(TARGET).x $(OBJECTS) -I$(INC)

clean:
	find . -name '*.o' -delete
	rm -rf *.x
