import matplotlib.pyplot as plt
import qimage2ndarray
import numpy as np
from mpl_toolkits.mplot3d import Axes3D


# fig = plt.figure()
# ax = fig.add_subplot(111, projection='3d')


def array_to_colormap_qim(a):
    im = plt.imshow(a)
    color_matrix = im.cmap(im.norm(im.get_array()))
    qim = qimage2ndarray.array2qimage(color_matrix, normalize=True)
    return qim


def array_to_colormap_qim_leap(a):
    im = plt.imshow(a)
    im = im.get_array()
    qim = qimage2ndarray.array2qimage(im, normalize=True)
    return qim


def array_to_3D_scatter_qim(a):
    # im = ax.scatter(a[:, 0], a[:, 1], a[:, 2], ',', c=a[:, 3], s=28,
    #                 marker='o')
    # im = plt.scatter(a[:, 0], a[:, 1], ',', c=a[:, 3], s=28,
    #                 marker='o')
    # color_matrix = im.cmap(im.norm(im.get_array()))
    # qim = qimage2ndarray.array2qimage(color_matrix, normalize=True)
    # return qim
    return 1


def rgba2rgb(rgba, background=(255, 255, 255)):
    """
    https://stackoverflow.com/questions/50331463/convert-rgba-to-rgb-in-python/50332356
    Parameters
    ----------
    rgba : object
    """
    row, col, ch = rgba.shape

    if ch == 3:
        return rgba

    assert ch == 4, 'RGBA image has 4 channels.'

    rgb = np.zeros((row, col, 3), dtype='float32')
    r, g, b, a = rgba[:, :, 0], rgba[:, :, 1], rgba[:, :, 2], rgba[:, :, 3]

    a = np.asarray(a, dtype='float32') / 255.0

    R, G, B = background

    rgb[:, :, 0] = r * a + (1.0 - a) * R
    rgb[:, :, 1] = g * a + (1.0 - a) * G
    rgb[:, :, 2] = b * a + (1.0 - a) * B

    return np.asarray(rgb, dtype='uint8')
