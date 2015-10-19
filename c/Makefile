CC=cc
CFLAGS=-O2 -DTEST -Wall
LFLAGS= -lpthread #-lblosc -L/usr/local/lib

objects = ra.o

all: ra

ra: $(objects)
	$(CC) $(LFLAGS) $(objects) -o ra

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o ra
