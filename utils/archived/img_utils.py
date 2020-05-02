import matplotlib.pyplot as plt
import qimage2ndarray
from mpl_toolkits.mplot3d import Axes3D

# fig = plt.figure()
# ax = fig.add_subplot(111, projection='3d')


def array_to_colormap_qim(a):
    im = plt.imshow(a)
    color_matrix = im.cmap(im.norm(im.get_array()))
    qim = qimage2ndarray.array2qimage(color_matrix, normalize=True)
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
