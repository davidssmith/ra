#
# This file is part of the RA package (http://github.com/davidssmith/ra).
#
# The MIT License (MIT)
#
# Copyright (c) 2015-2017 David Smith
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
#

ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif



CC=cc -std=c99
CFLAGS=-O2 -Wall -g
LFLAGS= -lm
H5FLAGS=-I/usr/include/hdf5/serial

objects = ra.o lz4.o

all: ra2cfl cfl2ra ra test timing

test: $(objects) test.o
	$(CC) $(objects) test.o -o test $(LFLAGS)
timing: $(objects) timing.o
	$(CC) $(objects) timing.o -o timing $(LFLAGS)
h5time: $(objects) h5time.c
	h5cc -O2 h5time.c -o h5time -lhdf5 $(H5FLAGS)
pngtime: pngtime.o ra.o lz4.o
	$(CC) $(objects) -O2 pngtime.c -o pngtime $(LFLAGS) -lpng 
ra2png: ra2png.o ra.o lz4.o
	$(CC) $(objects) -O2 ra2png.c -o ra2png $(LFLAGS) -lpng 
ra2cfl: $(objects)  ra2cfl.o
	$(CC) $(objects) ra2cfl.o -o ra2cfl $(LFLAGS)
cfl2ra: $(objects)  cfl2ra.o
	$(CC) $(objects) cfl2ra.o -o cfl2ra $(LFLAGS)
ra: $(objects) main.o
	$(CC) $(objects) main.o -o ra $(LFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o test ra2cfl cfl2ra ra timing a.out hdf5 pngtime

install: ra2cfl cfl2ra ra ra2png
	install -m 0755 ra2cfl $(PREFIX)/bin
	install -m 0755 cfl2ra $(PREFIX)/bin
	install -m 0755 ra2png $(PREFIX)/bin
	install -m 0755 ra $(PREFIX)/bin

