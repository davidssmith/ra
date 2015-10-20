function rawrite(data, filename)
w = whos('data');
elbytes = w.bytes / numel(data);
if isinteger(data)
    if strfind(w.class, 'uint')
        eltype = 2;
    else
        eltype = 1;
    end
elseif isfloat(data)
    if w.complex
        eltype = 4;
        %elbytes = elbytes / 2;
        re = real(data);
        im = imag(data);
        data = zeros([2,size(data)]);
        data(1:2:end) = re;
        data(2:2:end) = im;
        clear re im;
    else
        eltype = 3;
    end
else
    eltype = 0;
end
f = fopen(filename,'w');
flags = 0;
filemagic = 8746397786917265778;
fwrite(f, filemagic, 'uint64');
fwrite(f, flags, 'uint64');
fwrite(f, eltype, 'uint64');
fwrite(f, elbytes, 'uint64');
fwrite(f, w.bytes, 'uint64');
nd = ndims(data);
if w.complex, nd = nd -1; end
fwrite(f, nd, 'uint64');
dims = size(data);
if w.complex, dims = dims(2:end); end
fwrite(f, dims, 'uint64');
fwrite(f, data, w.class);
fclose(f);
