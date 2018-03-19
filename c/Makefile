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
CFLAGS=-O2 -Wall # -g -fPIC
LFLAGS= -lm #-lpthread -lblosc -L/usr/local/lib

objects = ra.o float16.o

all: ra2cfl cfl2ra rahead raconvert rasquash radiff radims rareshape test

test: $(objects) test.o
	$(CC) $(objects) test.o -o test $(LFLAGS)
ra2cfl: $(objects)  ra2cfl.o
	$(CC) $(objects) ra2cfl.o -o ra2cfl $(LFLAGS)
cfl2ra: $(objects)  cfl2ra.o
	$(CC) $(objects) cfl2ra.o -o cfl2ra $(LFLAGS)
rahead: $(objects) rahead.o
	$(CC) $(objects) rahead.o -o rahead $(LFLAGS)
raconvert: $(objects) raconvert.o
	$(CC) $(objects) raconvert.o -o raconvert $(LFLAGS)
rasquash: $(objects) rasquash.o
	$(CC) $(objects) rasquash.o -o rasquash $(LFLAGS)
radiff: $(objects) radiff.o
	$(CC) $(objects) radiff.o -o radiff $(LFLAGS)
radims: $(objects) radims.o
	$(CC) $(objects) radims.o -o radims $(LFLAGS)
rareshape: $(objects) rareshape.o
	$(CC) $(objects) rareshape.o -o rareshape $(LFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o test ra2cfl cfl2ra rahead raconvert rasquash radiff radims rareshape

install: ra2cfl cfl2ra rahead raconvert rasquash radiff radims rareshape
	install -m 0755 ra2cfl $(PREFIX)/bin
	install -m 0755 cfl2ra $(PREFIX)/bin
	install -m 0755 rahead $(PREFIX)/bin
	install -m 0755 raconvert $(PREFIX)/bin
	install -m 0755 rasquash $(PREFIX)/bin
	install -m 0755 radiff $(PREFIX)/bin
	install -m 0755 radims $(PREFIX)/bin
	install -m 0755 rareshape $(PREFIX)/bin

