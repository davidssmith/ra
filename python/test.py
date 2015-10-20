import ra

testfile1 = '../data/test.ra'
testfile2 = 'test2.ra'
data = ra.read(testfile1)
print data

ra.write(data, testfile2)
data2 = ra.read(testfile2)
print data2

print ra.query(testfile1)
print ra.query(testfile2)

assert (data == data2).all()
print 'TEST PASSED'
