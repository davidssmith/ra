using RawArray
using MAT
using HDF5
using BenchmarkTools

nfiles = 10000
n = 10
path = mktempdir() * "/"

println("Temporary files will be placed in $path.")
println("If code crashes, or is terminated early, but sure to delete that directory.")
function cleanup(n, nfiles)
	for i in 1:nfiles
		rm("$path/$i.ra", force=true)
		rm("$path/$i.h5", force=true)
		rm("$path/$i.mat", force=true)
	end
	rm("$path/big.ra", force=true)
	rm("$path/big.h5", force=true)
	rm("$path/small.h5", force=true)
	rm("$path/big.mat", force=true)
	rm("$path/small.mat", force=true)
end

function rasmalltest(x, n, nfiles)
	for i in 1:nfiles
		rawrite(x, "$path/$i.ra")
	end
	for i in 1:nfiles
		x = raread("$path/$i.ra")
	end
	return x[1]
end

function ralargetest(x, n, nfiles)
	rawrite(x, path*"big.ra")
	x = raread("$path/big.ra")
	return x[1]
end


function hdf5largetest(x, n, nfiles)
	h5write("$path/big.h5", "test/x", x)
	x = h5read("$path/big.h5", "test/x")
	return x[1]
end


function hdf5smalltestone(x, n, nfiles)
	h5open("$path/small.h5", "w") do file
		for i in 1:nfiles
			write(file, "a$i", x)  # alternatively, say "@write file A"
		end
	end
	c = h5open("$path/small.h5", "r") do file
		for i in 1:nfiles
			x = read(file, "a$i")
		end
	end
	return x[1]
end

function hdf5smalltestmany(x, n, nfiles)
	for i in 1:nfiles
		h5write("$path/$i.h5", "test/x", x)
	end
	for i in 1:nfiles
		x = h5read("$path/$i.h5", "test/x")
	end
	return x[1]
end

function matlargetest(x, n, nfiles)
	matwrite("$path/big.mat", Dict("x"=>x))
	x = matread("$path/big.mat")
	return x["x"]
end

function matsmalltestmany(x, n, nfiles)
	for i in 1:nfiles
		matwrite("$path/$i.mat", Dict("x"=> x))
	end
	for i in 1:nfiles
		x = matread("$path/$i.mat")
	end
	return x["x"]
end

xsmall = rand(n)
xbig = rand(nfiles, n, n, n)

print("MAT small separate:      ")
@time matsmalltestmany(xsmall, n, nfiles)
print("HDF5 small separate:     ")
@time hdf5smalltestmany(xsmall, n, nfiles)
print("HDF5 small together:     ")
@time hdf5smalltestone(xsmall, n, nfiles)
print("RawArray small separate: ")
@time rasmalltest(xsmall, n, nfiles)
print("MAT large:               ")
@time matlargetest(xbig, n, nfiles)
print("HDF5 large:              ")
@time hdf5largetest(xbig, n, nfiles)
print("RawArray large:          ")
@time ralargetest(xbig, n, nfiles)
cleanup(n, nfiles)

