import matplotlib.pyplot as plt
import qimage2ndarray
from PyQt5.QtGui import QPixmap
import numpy as np
from mpl_toolkits.mplot3d import Axes3D

import numpy as np
import pyqtgraph as pg

# fig = plt.figure()
# ax = fig.add_subplot(111, projection='3d')


def array_to_colormap_qim(a, normalize=True):
    im = plt.imshow(a)
    color_matrix = im.cmap(im.norm(im.get_array()))
    qim = qimage2ndarray.array2qimage(color_matrix, normalize=normalize)
    return qim


def array_to_colormap_qim_leap(a):
    im = plt.imshow(a)
    im = im.get_array()
    qim = qimage2ndarray.array2qimage(im, normalize=True)
    return qim


def array_to_colormap_qim_leap(a):
    im = plt.imshow(a)
    im = im.get_array()
    qim = qimage2ndarray.array2qimage(im, normalize=True)
    return qim


def array_to_data3D_scatter_qim(a):
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


def process_clutter_removed_spectrogram(spec_array, vmax, vmin, num_rows, height=128, width=128):
    spec_array_scaled = np.concatenate(
        (np.expand_dims(np.linspace(vmax, vmin, num_rows), axis=-1), spec_array), axis=-1)
    heatmap_qim = array_to_colormap_qim(spec_array_scaled, normalize=True)
    qpixmap = QPixmap(heatmap_qim)
    qpixmap = qpixmap.scaled(width, height, pg.QtCore.Qt.KeepAspectRatio)  # resize spectrogram
    return qpixmap


def plot_spectrogram(spec_array, height=128, width=128):
    heatmap_qim = array_to_colormap_qim(spec_array, normalize=True)
    qpixmap = QPixmap(heatmap_qim)
    qpixmap = qpixmap.scaled(width, height, pg.QtCore.Qt.KeepAspectRatio)  # resize spectrogram
    return qpixmap

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
