import pickle

import numpy as np
import pandas as pd
from scipy.spatial import distance
from sklearn.cluster import DBSCAN
from sklearn.metrics import confusion_matrix
import matplotlib.pyplot as plt

import time

from sklearn.preprocessing import MinMaxScaler
from sklearn.utils.multiclass import unique_labels

from utils.transformation import translate, sphere_search, rotateZ, rotateY, rotateX, scale


volume_shape = [25, 25, 25]


def plot_confusion_matrix(y_true, y_pred, classes,
                          normalize=False,
                          title=None,
                          cmap=plt.cm.Blues):
    """
    This function prints and plots the confusion matrix.
    Normalization can be applied by setting `normalize=True`.
    """
    if not title:
        if normalize:
            title = 'Normalized confusion matrix'
        else:
            title = 'Confusion matrix, without normalization'

    # Compute confusion matrix
    cm = confusion_matrix(y_true, y_pred)
    # Only use the labels that appear in the data
    classes = classes[unique_labels(y_true, y_pred)]
    if normalize:
        cm = cm.astype('float') / cm.sum(axis=1)[:, np.newaxis]
        print("Normalized confusion matrix")
    else:
        print('Confusion matrix, without normalization')

    print(cm)

    fig, ax = plt.subplots()
    im = ax.imshow(cm, interpolation='nearest', cmap=cmap)
    ax.figure.colorbar(im, ax=ax)
    # We want to show all ticks...
    ax.set(xticks=np.arange(cm.shape[1]),
           yticks=np.arange(cm.shape[0]),
           # ... and label them with the respective list entries
           xticklabels=classes, yticklabels=classes,
           title=title,
           ylabel='True label',
           xlabel='Predicted label')

    # Rotate the tick labels and set their alignment.
    plt.setp(ax.get_xticklabels(), rotation=45, ha="right",
             rotation_mode="anchor")

    # Loop over data dimensions and create text annotations.
    fmt = '.2f' if normalize else 'd'
    thresh = cm.max() / 2.
    for i in range(cm.shape[0]):
        for j in range(cm.shape[1]):
            ax.text(j, i, format(cm[i, j], fmt),
                    ha="center", va="center",
                    color="white" if cm[i, j] > thresh else "black")
    fig.tight_layout()
    return ax

DBSCAN_esp = 0.2
DBSCAN_minSamples = 3
def produce_voxel(points, isCluster=True, isClipping=False):
    """

    :param frame: np a with input shape (n, 4)
    :return voxel
    """

    if len(points) == 0:  # if there's no detected points
        return np.zeros(tuple([1] + volume_shape))

    points_new = np.asarray([x for x in points if 1.0 > x[3] > -1.0])
    if not np.all(points_new == points):
        print('Warning: point VELOCITY out of bound')
    points = points_new

    if isCluster:
        # take off the doppler for clustering
        doppler_col = np.copy(points[:, 3])
        points[:, 3] = np.zeros(points[:, 3].shape)
        db = DBSCAN(eps=DBSCAN_esp, min_samples=DBSCAN_minSamples).fit(points)
        # append back the doppler
        points[:, 3] = doppler_col

        core_samples_mask = np.zeros_like(db.labels_, dtype=bool)
        core_samples_mask[db.core_sample_indices_] = True
        labels = db.labels_

        unique_labels = set(labels)
        clusters = []
        for k in zip(unique_labels):
            if k == -1:
                # Black used for noise.
                col = [0, 0, 0, 1]
            class_member_mask = (labels == k)
            xyz = points[class_member_mask & core_samples_mask]
            if xyz.any():  # in case there are none objects
                clusters.append(xyz)  # append this cluster data to the cluster list
            # each cluster is a 3 * n matrix
            xyz = points[class_member_mask & ~core_samples_mask]

        clusters.sort(key=lambda xyz: distance.euclidean((0.0, 0.0, 0.0), np.array(
            [np.mean(xyz[:, 0]), np.mean(xyz[:, 1]), np.mean(xyz[:, 2])])))

        #############################
        hand_cluster = []
        if len(clusters) > 0:
            hand_cluster = clusters[0]

    else:
        hand_cluster = points

    hand_cluster = np.array(hand_cluster)
    frame_3D_volume = snapPointsToVolume(hand_cluster, volume_shape, isClipping=isClipping)

    return np.expand_dims(frame_3D_volume, axis=0)

xmin, xmax = -0.255, 0.255
ymin, ymax = 0.0, 0.255
zmin, zmax = -0.255, 0.255

heatMin, heatMax = -1.0, 1.0
xyzScaler = MinMaxScaler().fit(np.array([[xmin, ymin, zmin],
                                         [xmax, ymax, zmax]]))
heatScaler = MinMaxScaler().fit(np.array([[heatMin],
                                          [heatMax]]))

def snapPointsToVolume(points, volume_shape, isClipping=False, radius=3, decay=0.8):
    """
    make sure volume is a square
    :param points: n * 4 a
    :param heat: scale 0 to 1
    :param volume:
    """
    assert len(volume_shape) == 3 and volume_shape[0] == volume_shape[1] == volume_shape[2]
    volume = np.zeros(volume_shape)

    if len(points) != 0:

        # filter out points that are outside the bounding box
        # using ABSOLUTE normalization

        points[:, :3] = xyzScaler.transform(points[:, :3])
        points[:, 3:] = heatScaler.transform(points[:, 3:])

        size = volume_shape[0]  # the length of the square side
        axis = np.array((size - 1) * points[:, :3], dtype=int)  # size minus 1 for index starts at 0

        for i, row in enumerate(points):
            heat = row[3]

            try:
                volume[axis[i][0], axis[i][1], axis[i][2]] = volume[axis[i][0], axis[i][1], axis[i][2]] + heat
            except IndexError:
                print('Index Out of Bound!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!')
            if isClipping:
                point_to_clip = sphere_search(shape=volume_shape, index=(axis[i][0], axis[i][1], axis[i][2]), r=radius)
                for dist, ptc in point_to_clip:
                    if dist != 0.0:
                        factor = (radius - dist + 1) * decay /radius
                        volume[ptc[0], ptc[1], ptc[2]] = volume[ptc[0], ptc[1], ptc[2]] + heat * factor

    volume_mean = np.mean(volume)
    assert volume_mean < 0.1
    assert volume_mean > -0.1

    return volume


# frameArray = np.load('F:/test_frameArray.npy')
# start = time.time()
# result = preprocess_frame(frameArray[2])
# end = time.time()
# print('Preprocessing frame took ' + str(end-start))

def merge_dict(dicts: list):
    merged_dict = dict()
    merged_len = 0
    for d in dicts:
        merged_len += len(d)
        merged_dict = {**merged_dict, **d}

    # make sure there is no replacement of elements
    try:
        assert merged_len == len(merged_dict)
    except AssertionError as ae:
        print(str(ae))
        raise Exception('dict item replaced!')
    return merged_dict


class StreamingMovingAverage:
    def __init__(self, window_size):
        self.window_size = window_size
        self.values = []
        self.sum = 0

    def process(self, value):
        self.values.append(value)
        self.sum += value
        if len(self.values) > self.window_size:
            self.sum -= self.values.pop(0)
        return float(self.sum) + value / len(self.values)


def parse_deltalize_recording(file: str) -> dict:
    recording_list = list()
    xyz_array = list()
    lines = list()
    with open(file) as fp:  # first buffer all the lines so that the buffered lines supports indexing
        for l in fp:
            lines.append(l)

    for i, buffered_l in enumerate(lines):
        timestamp, x, y, z = [float(x) for x in buffered_l.split(',')]

        if i == 0:  # if this is the first frame
            x, y, z = 0, 0, 0
        else:
            # process delta
            prev_x, prev_y, prev_z = [float(x) for x in lines[i-1].split(',')][1:]
            x, y, z = x - prev_x, y - prev_y, z-prev_z

        xyz_array.append([x, y, z])
        recording_list.append([timestamp, (x, y, z)])

    assert len(xyz_array) == len(recording_list)
    return dict(recording_list), np.asarray(xyz_array)


def linear_process(a: float, b: float, x: float):
    return x * a + b


class Queue:
    def __init__(self, maxlen):
        self.data = list()
        self.maxlen = maxlen

    def __getitem__(self, key):
        # It's probably better to catch any IndexError to at least provide
        # a class-specific exception
        return self.data[key]

    def get_list(self):
        return self.data

    def __len__(self):
        return len(self.data)

    def push_right(self, d):
        self.data = self.data[-self.maxlen + 1:] + [d]

    def pop_right(self):
        if len(self.data) > 0:
            temp = self.data[-1]
            self.data = self.data[:-1]
            return temp
        else:
            return None