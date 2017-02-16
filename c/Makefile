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


CC=cc -std=c99
CFLAGS=-O2 -Wall -g
LFLAGS= -lm #-lpthread -lblosc -L/usr/local/lib

objects = ra.o

all: ra2cfl cfl2ra rahead raconvert rasquash radiff test

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
radiff: $(objects) radiff.o
	$(CC) $(LFLAGS) $(objects) radiff.o -o radiff

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o test ra2cfl cfl2ra rahead raconvert rasquash radiff
