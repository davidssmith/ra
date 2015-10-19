
include("ra.jl")
using RA
using Base.Test

testfile1 = "test.ra"
testfile2 = "test2.ra"

println("reading $testfile1...")
tic()
data1 = raread(testfile1)
truth = reshape(Float32[0:length(data1)-1;]-im*(1./Float32[0:length(data1)-1;]),3,4)
@test isequal(data1, truth)
println("writing $testfile2...")
rawrite(data1, testfile2)
println("reading $testfile2...")
data2 = raread(testfile2)
toc()
println("querying ...")
q1 = raquery(testfile1)
q2 = raquery(testfile2)
println(q1)
println(q2)

@test isequal(data1, data2)
println("PASSED")
