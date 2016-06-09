% 1024x0124x128
% 
% mat: 1.72 s
% ra: 1.47 s
% hdf5: 
% 

tic;
save test x;
toc;
%%

tic;
rawrite(x,'test.ra');
toc;
%%
tic;
h5create('test.h5','/data',size(x));
h5write('test.h5','/data',x);
toc;

%%
!rm test.h5
