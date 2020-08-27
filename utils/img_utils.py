import matplotlib.pyplot as plt
import qimage2ndarray
from PyQt5.QtGui import QPixmap
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import pyqtgraph as pg
# fig = plt.figure()
# ax = fig.add_subplot(111, projection='3d')
import config


def array_to_colormap_qim(a, normalize=True):
    im = plt.imshow(a)
    color_matrix = im.cmap(im.norm(im.get_array()))
    qim = qimage2ndarray.array2qimage(color_matrix, normalize=normalize)
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