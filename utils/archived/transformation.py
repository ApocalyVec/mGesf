import numpy as np
import itertools as it
from scipy.spatial import distance

def transform(m, trans_m):
    """
    NOTE: this function is called by other transformation functions, do not call this function directly
    :param m: the n * 3 matrix for which to apply translate
    :param trans_m: the 4 * 4 transformation matrix depending on the transformation type
    :return: the transformed matrix m
    """

    result = np.zeros((m.shape[0], m.shape[1]))

    for row in range(m.shape[0]):
        # a single point [x, y, z, 1]
        tmp = np.append(m[row], 1.).reshape(4, 1)
        # apply translation to a single point
        result[row] = np.delete(np.matmul(trans_m, tmp).reshape(-1), 3)
    return result


def translate(m, x, y, z):
    """

    :param m: the n * 3 matrix for which to apply translate
    :param x:
    :param y:
    :param z:
    """
    translation_matrix = np.array([
        [1., 0., 0., x],
        [0., 1., 0., y],
        [0., 0., 1., z],
        [0., 0., 0., 1.]
    ])
    return transform(m, translation_matrix)


def scale(m, x, y, z):
    """

    :param m: the n * 3 matrix for which to apply scale
    :param x:
    :param y:
    :param z:
    """
    scaling_matrix = np.array([
        [x, 0., 0., 0.],
        [0., y, 0., 0.],
        [0., 0., z, 0.],
        [0., 0., 0., 1.]
    ])
    center = np.mean(m, axis=0)
    offset_x = center[0]
    offset_y = center[1]
    offset_z = center[2]

    tmp = translate(m, -offset_x, -offset_y, -offset_z)
    tmp = transform(tmp, scaling_matrix)
    return translate(tmp, offset_x, offset_y, offset_y)


def rotateX(m, theta):
    """

    :param m: the n * 3 matrix for which to apply scale
    :param x:
    :param y:
    :param z:
    """
    theta_radian = np.radians(theta)
    rotation_matrix = np.array([
        [1., 0., 0., 0.],
        [0., np.cos(theta_radian), -np.sin(theta_radian), 0.],
        [0., np.sin(theta_radian), np.cos(theta_radian), 0.],
        [0., 0., 0., 1.]
    ])
    return transform(m, rotation_matrix)


def rotateY(m, theta):
    """

    :param m: the n * 3 matrix for which to apply scale
    :param x:
    :param y:
    :param z:
    """
    theta_radian = np.radians(theta)
    rotation_matrix = np.array([
        [np.cos(theta_radian), 0., np.sin(theta_radian), 0.],
        [0., 1., 0., 0.],
        [-np.sin(theta_radian), 0., np.cos(theta_radian), 0.],
        [0., 0., 0., 1.]
    ])
    return transform(m, rotation_matrix)


def rotateZ(m, theta):
    """

    :param m: the n * 3 matrix for which to apply scale
    :param x:
    :param y:
    :param z:
    """
    theta_radian = np.radians(theta)
    rotation_matrix = np.array([
        [np.cos(theta_radian), -np.sin(theta_radian), 0., 0.],
        [np.sin(theta_radian), np.cos(theta_radian), 0., 0.],
        [0., 0., 1., 0.],
        [0., 0., 0., 1.]
    ])
    return transform(m, rotation_matrix)


def sphere_search(shape, index, r):
    """

    :param m: the 3*3*3 matrix where search will be done
    :param index: the center index, a tuple
    :param r: the search radius
    :return: a list of (distance, index) pairs for indices within radius
    """

    x, y, z = shape
    index_x, index_y, index_z = index
    x_min = max(0, index_x - r)
    if x > index_x + r:
        x_max = index_x + r + 1
    else:
        x_max = x
    y_min = max(0, index_y - r)
    if y > index_y + r:
        y_max = index_y + r + 1
    else:
        y_max = y
    z_min = max(0, index_z - r)
    if z > index_z + r:
        z_max = index_z + r + 1
    else:
        z_max = z

    points_to_look = it.product(*[range(x_min, x_max), range(y_min, y_max), range(z_min, z_max)])

    return ((distance.euclidean((x, y, z), index), (x, y, z)) for x, y, z in points_to_look if distance.euclidean((x, y, z), index) <= r)


# if __name__ == '__main__':
#     import matplotlib.pyplot as plt
#     from mpl_toolkits.mplot3d import Axes3D
#
#     arr = np.random.rand(100, 3)
#
#     fig = plt.figure()
#     ax = Axes3D(fig)
#
#     ax.scatter(arr[:, 0], arr[:, 1], arr[:, 2])
#
#     # result = translate(arr, 2, 2, 2)
#     result = scale(arr, 3, 3, 3)
#     # result = rotateX(arr, 180)
#
#     ax.scatter(result[:, 0], result[:, 1], result[:, 2])
#
#     result2 = scale(arr, 2, 2, 2)
#     ax.scatter(result2[:, 0], result2[:, 1], result2[:, 2])
#
#     result3 = scale(arr, 2.5, 2.5, 2.5)
#     ax.scatter(result3[:, 0], result3[:, 1], result3[:, 2])
#
#     plt.show()
#
