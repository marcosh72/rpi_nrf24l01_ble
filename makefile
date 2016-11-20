TARGET=bbble

PREFIX=arm-linux-gnueabihf-
CC=$(PREFIX)g++

INC=./inc
SRC=./src
LIB=./lib

CFLAGS=-O2 -pipe -Wall -I$(INC) -march=armv6zk
LDFLAGS=-L$(LIB) -lbcm2835

SOURCES=$(shell find . -name '*.cpp')
OBJECTS=$(patsubst %.cpp,%.o,$(SOURCES))

%.o: %.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

all: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET).x $(OBJECTS) $(LDFLAGS)

clean:
	find . -name '*.o' -delete
	rm -rf *.x
