Welcome to the RA package!
===========================

Introduction
------------

RA is a simple file format for storing n-D arrays. RA stands for **raw array**
and should be pronounced *arr-ay*, although it is not a coincidence that the
mispronounciation *rah* (as in "raw" in some dialects) also makes sense.

RA was designed to be portable, fast, and storage
efficient. For scientific applications in particular, it can allow the simple
storage of large n-D data arrays without a separate header file to store the
dimensions and type metadata. 

I believe the world doesn't need another hierarchical data format. We already have one of those -- it's called a filesystem. What is needed is a simple one-to-one mapping of data structures to disk files that preserves metadata and is fast and simple to read and write.

In addition to `float`s, `int`s, and `uint`s of a range of sizes, RA also supports

(1) **Native complex floats**: which other common formats, such as HDF5, don't have.

(2) **User-defined data structures**: however, the encoding and decoding of those is left to the end user, since only they can know the structure of their `struct`. 

As an aside, the RA format is technically recursive (or even fractal?) -- you could store an array of RA files in a RA file if you want, although the I/O functions don't support that out of the box.

Getting
-------

You can clone the git repository on your local machine if you have git installed using the command 

```
git clone https://github.com/davidssmith/ra.git
```

If you don't have git or don't want it, you can also download individual files through the website interface or the whole thing as a [zip archive](https://github.com/davidssmith/ra/archive/master.zip).

Usage
-----

### C

Only the C version needs to be compiled. Probably the easiest way to use that version is to drop the source files `ra.c` and `ra.h` into your project source directory, include the header file `ra.h`, and compile it along with your own project.  A makefile is also provided with which you can compile the demo and run it. You can also modify the Makefile to compile a shared library, if you so wish.

To run the demo, clone or download the git archive, navigate to the `ra/c` directory, and run `make`.  If the make fails, try to fix it for your particular compiler. I've provided a fairly generic one, though, so it should work on most systems. If the fix is not obvious, file an issue on the git repository, and I'll try to help.

If the make succeeds, you can run the demo with `./ra`.  If the tests pass, you'll see a message at the end to that effect. The output from the demo should look like this:

```
test data is 12 floats
0.000000+-infim
1.000000+-1.000000im
2.000000+-0.500000im
3.000000+-0.333333im
4.000000+-0.250000im
5.000000+-0.200000im
6.000000+-0.166667im
7.000000+-0.142857im
8.000000+-0.125000im
9.000000+-0.111111im
TESTS PASSED!
---
name: test.ra
endian: little
type: complex64
size: 96
dimension: 2
shape:
  - 3
  - 4
...
```

### Julia

To use the Julia version, add the following lines to your Julia code:

```
include("ra.jl")
using RA
```

Now you can call `raread` and `rawrite` for Julia objects of type `Array{T,N}`.

A test file called `test.jl` has been included.  You can test the code on your machine at the command line by running `julia test.jl`. If the tests pass, you'll get a message saying so. The test output should look like this:

```
reading test.ra...
writing test2.ra...
reading test2.ra...
elapsed time: 0.32623855 seconds
querying ...
---
name: test.ra
endian: little
type: Complex{Float32}
size: 96
dimension: 2
shape:
  - 3
  - 4
...
---
name: test2.ra
endian: little
type: Complex{Float32}
size: 96
dimension: 2
shape:
  - 3
  - 4
...
PASSED
```

### Matlab

Drop the files `raread.m` and `rawrite.m` into your preferred directory in your Matlab path. Now you can call `array = raread(filename)` to return an array and `rawrite(array, filename)` to save an array to a file.

Getting Help
------------

If you need help, file an issue on the [bug tracker](http://github.com/davidssmith/ra/issues).

Authors
-------
David S. Smith <david.smith@gmail.com>

Disclaimer
----------
This code comes with no warranty. Use at your own risk. If it breaks, instead of suing me, please help me fix it. Thank you.