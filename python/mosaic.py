def mosaic(img, pad=True):
    """
    Create a 2-D mosaic of images from an n-D image. An attempt is made to
    make the resulting 2-D image as square as possible.

    Parameters
    ----------
    img : ndarray
        n-dimensional image be tiled into a mosaic. All but last two dims are
        lumped.

    Returns
    -------
    mosaic : 2-d image
        Tiled mosaic of images.
    """
    from numpy import ix_, array, zeros, arange
    from math import sqrt, floor, ceil
    if len(img.shape) <= 2:   # already 2-D, so skip the rest
        return img
    img = array(img)
    nr, nc = img.shape[-2:]    # take off last two dimensions, rest are lumped.
    print('img.shape=',img.shape,end=None)
    img = img.reshape((-1, nr, nc))
    print(' -> ', img.shape)
    nz = img.shape[0]

    if pad:
        n = int(ceil(sqrt(nz)))  # starting guess for tiling dimensions
        M = zeros((nr*n, nc*n), dtype=img.dtype)
        for j2 in range(n):      # stick them together
            for j1 in range(n):  # there is probably a better way to do this
                if j1+j2*n >= nz:
                    break
                rows = nr*j2 + arange(nr)
                cols = nc*j1 + arange(nc)
                M[ix_(rows, cols)] = img[j1+j2*n, :, :]
        return M
    else:
        n = int(floor(sqrt(nz)))  # starting guess for tiling dimensions
        # find largest integer less than or equal to sqrt that evenly divides the
        # number of 2-d images in the n-d image.
        m = [x for x in range(1, n+1) if nz % x == 0]
        m = m[-1]
        j = nz // m      # figure out the most square dimensions
        n2 = min(j, m)
        n1 = max(j, m)
        M = zeros((nr*n2, nc*n1), dtype=img.dtype)
        for j2 in range(n2):      # stick them together
            for j1 in range(n1):  # there is probably a better way to do this
                rows = nr*j2 + arange(nr)
                cols = nc*j1 + arange(nc)
                M[ix_(rows, cols)] = img[j1+j2*n1, :, :]
        return M


def main():
    import scipy.misc
    import matplotlib.pyplot as pl
    import numpy as np
    img = scipy.misc.face()
    nz = 64
    n = img.shape[0]
    img = np.transpose(np.tile(img, [1, 1, nz]), (2,0,1))
    print(f'img.shape={img.shape}')
    pl.imshow(mosaic(img), cmap='gray')
    pl.show()


if __name__ == '__main__':
    main()

