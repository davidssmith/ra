function data = raread(filename)
fd = fopen(filename,'r');
h = getheader(fd);
ra_type_names = {'user','int','uint','float','complex'};
if h.eltype == 4 % complex float, which Matlab can't handle
    wascomplex = 1;
    h.elbyte = h.elbyte / 2;
    h.dims = [2; h.dims];
    h.eltype = 3;
else 
    wascomplex = 0;
end
ratype = sprintf('%s%d',ra_type_names{h.eltype+1}, h.elbyte*8);
fprintf('think i got %s\n', ratype);
data = fread(fd, h.elbyte*prod(h.dims), ratype);
if wascomplex
    tmp = complex(data(1:2:end), data(2:2:end));
    data = reshape(tmp, h.dims(2:end).');
else
    data = reshape(data, h.dims.');
end

function st = getheader(fd)
h = fread(fd, 5, 'uint64');  
st = {};
st.flags = h(1);
st.eltype = h(2);
st.elbyte = h(3);
st.size = h(4);
st.ndims = h(5);
st.dims = fread(fd, st.ndims, 'uint64');