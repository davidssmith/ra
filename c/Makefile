CC=cc -std=c99
CFLAGS=-O2 -Wall -g
LFLAGS= #-lpthread -lblosc -L/usr/local/lib

objects = ra.o

all: ra2cfl cfl2ra rahead raconvert rasquash test

test: $(objects) test.o
	$(CC) $(LFLAGS) $(objects) test.o -o test
ra2cfl: $(objects)  ra2cfl.o
	$(CC) $(LFLAGS) $(objects) ra2cfl.o -o ra2cfl
cfl2ra: $(objects)  cfl2ra.o
	$(CC) $(LFLAGS) $(objects) cfl2ra.o -o cfl2ra
rahead: $(objects) rahead.o
	$(CC) $(LFLAGS) $(objects) rahead.o -o rahead
raconvert: $(objects) raconvert.o
	$(CC) $(LFLAGS) $(objects) raconvert.o -o raconvert
rasquash: $(objects) rasquash.o
	$(CC) $(LFLAGS) $(objects) rasquash.o -o rasquash

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o test ra2cfl cfl2ra rahead raconvert rasquash
