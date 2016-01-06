CC=cc -std=c99
CFLAGS=-O2 -Wall
LFLAGS= -lpthread #-lblosc -L/usr/local/lib

objects = ra.o

all: demo ra2cfl cfl2ra

demo: $(objects) demo.o
	$(CC) $(LFLAGS) $(objects) demo.o -o demo

ra2cfl: $(objects)  ra2cfl.o
	$(CC) $(LFLAGS) $(objects) ra2cfl.o -o ra2cfl
cfl2ra: $(objects)  cfl2ra.o
	$(CC) $(LFLAGS) $(objects) cfl2ra.o -o cfl2ra

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o demo ra2cfl cfl2ra
