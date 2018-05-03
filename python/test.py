

import os
import ra


datadir = '../data/'

print(f'Testing read/write correctness of files in {datadir}:')
for root, dirs, files in os.walk(datadir):
    for f in files:
        if '.ra' in f:
            data = ra.read(datadir + f)
            print(f'\t{f} ({data.dtype}) ... ', end='')
            ra.write(data, 'test.ra')
            data2 = ra.read('test.ra')
            print((data == data2).all())
