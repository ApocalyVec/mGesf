import numpy as np
import pickle
import os
import shutil

import matplotlib.pyplot as plt
from matplotlib import style

from PIL import Image, ImageDraw, ImageFont
from sklearn.cluster import DBSCAN

from scipy.spatial import distance

from utils.path_utils import snapPointsToVolume

# IndexPen

# zr 0 ######################################################
radarData_path = 'F:/indexPen/data/f_data_zr_0/f_data.p'
videoData_path = 'F:/indexPen/data/v_data_zr_0/cam2'
mergedImg_path = 'F:/indexPen/figures/zr_0'
out_path = 'F:/indexPen/csv/zr_0'
# zr 1 ######################################################
# radarData_path = 'F:/indexPen/data/f_data_zr_1/f_data.p'
# videoData_path = 'F:/indexPen/data/v_data_zr_1/cam2'
# mergedImg_path = 'F:/indexPen/figures/zr_1'
# out_path = 'F:/indexPen/csv/zr_1'

# py 0 ######################################################
# radarData_path = 'F:/indexPen/data/f_data_py_0/f_data.p'
# videoData_path = 'F:/indexPen/data/v_data_py_0/cam2'
# mergedImg_path = 'F:/indexPen/figures/py_0'
# out_path = 'F:/indexPen/csv/py_0'
# py 1 ######################################################
# radarData_path = 'F:/indexPen/data/f_data_py_1/f_data.p'
# videoData_path = 'F:/indexPen/data/v_data_py_1/cam2'
# mergedImg_path = 'F:/indexPen/figures/py_1'
# out_path = 'F:/indexPen/csv/py_1'

# ya 0 ######################################################
# radarData_path = 'F:/indexPen/data/f_data_ya_0/f_data.p'
# videoData_path = 'F:/indexPen/data/v_data_ya_0/cam2'
# mergedImg_path = 'F:/indexPen/figures/ya_0'
# out_path = 'F:/indexPen/csv/ya_0'
# ya 1 ######################################################
# radarData_path = 'F:/indexPen/data/f_data_ya_1/f_data.p'
# videoData_path = 'F:/indexPen/data/v_data_ya_1/cam2'
# mergedImg_path = 'F:/indexPen/figures/ya_1'
# out_path = 'F:/indexPen/csv/ya_1'
# ya 2 ######################################################
# radarData_path = 'F:/indexPen/data/f_data_ya_2/f_data.p'
# videoData_path = 'F:/indexPen/data/v_data_ya_2/cam2'
# mergedImg_path = 'F:/indexPen/figures/ya_2'
# out_path = 'F:/indexPen/csv/ya_2'
# ya 3 ######################################################
# radarData_path = 'F:/indexPen/data/f_data_ya_3/f_data.p'
# videoData_path = 'F:/indexPen/data/v_data_ya_3/cam2'
# mergedImg_path = 'F:/indexPen/figures/ya_3'
# out_path = 'F:/indexPen/csv/ya_3'

# zl 0 ######################################################
# radarData_path = 'F:/indexPen/data/f_data_zl_0/f_data.p'
# videoData_path = 'F:/indexPen/data/v_data_zl_0/cam2'
# mergedImg_path = 'F:/indexPen/figures/zl_0'
# out_path = 'F:/indexPen/csv/zl_0'
# zl 1 ######################################################
# radarData_path = 'F:/indexPen/data/f_data_zl_1/f_data.p'
# videoData_path = 'F:/indexPen/data/v_data_zl_1/cam2'
# mergedImg_path = 'F:/indexPen/figures/zl_1'
# out_path = 'F:/indexPen/csv/zl_1'
# # zl 2 ######################################################
# radarData_path = 'F:/indexPen/data/f_data_zl_2/f_data.p'
# videoData_path = 'F:/indexPen/data/v_data_zl_2/cam2'
# mergedImg_path = 'F:/indexPen/figures/zl_2'
# out_path = 'F:/indexPen/csv/zl_2'
# # zl 3 ######################################################
# radarData_path = 'F:/indexPen/data/f_data_zl_3/f_data.p'
# videoData_path = 'F:/indexPen/data/v_data_zl_3/cam2'
# mergedImg_path = 'F:/indexPen/figures/zl_3'
# out_path = 'F:/indexPen/csv/zl_3'

# zy 0 ######################################################
# radarData_path = 'F:/indexPen/data/f_data_zy_0/f_data.p'
# videoData_path = 'F:/indexPen/data/v_data_zy_0/cam2'
# mergedImg_path = 'F:/indexPen/figures/zy_0'
# out_path = 'F:/indexPen/csv/zy_0'
# zy 1 ######################################################
# radarData_path = 'F:/indexPen/data/f_data_zy_1/f_data.p'
# videoData_path = 'F:/indexPen/data/v_data_zy_1/cam2'
# mergedImg_path = 'F:/indexPen/figures/zy_1'
# out_path = 'F:/indexPen/csv/zy_1'
# zy 2 ######################################################
# radarData_path = 'F:/indexPen/data/f_data_zy_2/f_data.p'
# videoData_path = 'F:/indexPen/data/v_data_zy_2/cam2'
# mergedImg_path = 'F:/indexPen/figures/zy_2'
# out_path = 'F:/indexPen/csv/zy_2'
# zy 3 ######################################################
# radarData_path = 'F:/indexPen/data/f_data_zy_3/f_data.p'
# videoData_path = 'F:/indexPen/data/v_data_zy_3/cam2'
# mergedImg_path = 'F:/indexPen/figures/zy_3'
# out_path = 'F:/indexPen/csv/zy_3'

# utility directory to save the pyplots
isplot = False

radar_3dscatter_path = 'F:/indexPen/figures/utils/radar_3dscatter'

radar_data = list(pickle.load(open(radarData_path, 'rb')).items())
radar_data.sort(key=lambda x: x[0])  # sort by timestamp
videoData_list = os.listdir(videoData_path)
videoData_timestamps = list(map(lambda x: float(x.strip('.jpg')), videoData_list))

style.use('fivethirtyeight')
white_color = 'rgb(255, 255, 255)'
black_color = 'rgb(0, 0, 0)'
red_color = 'rgb(255, 0, 0)'

DBSCAN_esp = 0.2
DBSCAN_minSamples = 3

# input data for the classifier that has the shape n*4*100, n being the number of samples
num_padding = 100
data_for_classifier = np.zeros((len(radar_data), num_padding, 4))
data_for_classifier_flattened = np.zeros(
    (len(radar_data), 4 * num_padding + 1 + 1 + 1))  # + 1 + 1 for the timestamp as integer ratio

fnt = ImageFont.truetype("arial.ttf", 16)

# Retrieve the first timestamp
starting_timestamp = radar_data[0][0]

interval_index = 1

# removed and recreate the merged image folder
if isplot:
    if os.path.isdir(mergedImg_path):
        shutil.rmtree(mergedImg_path)
    os.mkdir(mergedImg_path)

volume_shape = (25, 25, 25)

interval_volume_list = []
volumes_for_this_interval = []

interval_sec = 5
sample_per_sec = 20
sample_per_interval = interval_sec * sample_per_sec

print('Label Cheat-sheet:')
print('1 for A')
print('4 for D')
print('12 for L')
print('13 for M')
print('16 for P')

label_array = []

num_write = 2
this_label = 1.0

for i, radarFrame in enumerate(radar_data):

    # retrieve the data
    timestamp, fData = radarFrame

    # calculate the interval
    if (timestamp - starting_timestamp) >= 5.0:
        num_intervaled_samples = len(volumes_for_this_interval)
        if num_intervaled_samples < sample_per_interval / 4:
            raise Exception('Not Enough Data Points, killed')

        # decide the label
        if num_write == 1:
            if interval_index % (5 * num_write) == 1:
                this_label = 1.0
            elif interval_index % (5 * num_write) == 2:
                this_label = 4.0  # for label D
            elif interval_index % (5 * num_write) == 3:
                this_label = 12.0  # for label L
            elif interval_index % (5 * num_write) == 4:
                this_label = 13.0  # for label M
            elif interval_index % (5 * num_write) == 0:
                this_label = 16.0  # for label P
        elif num_write == 2:
            if interval_index % (5 * num_write) == 1 or interval_index % (5 * num_write) == 2:
                this_label = 1.0
            elif interval_index % (5 * num_write) == 3 or interval_index % (5 * num_write) == 4:
                this_label = 4.0  # for label D
            elif interval_index % (5 * num_write) == 5 or interval_index % (5 * num_write) == 6:
                this_label = 12.0  # for label L
            elif interval_index % (5 * num_write) == 7 or interval_index % (5 * num_write) == 8:
                this_label = 13.0  # for label M
            elif interval_index % (5 * num_write) == 9 or interval_index % (5 * num_write) == 0:
                this_label = 16.0  # for label P
        label_array.append(this_label)  # for label A

        print('Label for the last interval is ' + str(this_label) + ' Num Samples: ' + str(
            len(volumes_for_this_interval)))
        print('')

        # add padding, pre-padded
        if len(volumes_for_this_interval) < sample_per_interval:
            while len(volumes_for_this_interval) < sample_per_interval:
                volumes_for_this_interval.insert(0, np.expand_dims(np.zeros(volume_shape), axis=0))
        elif len(volumes_for_this_interval) > sample_per_interval:  # we take only the 75 most recent
            volumes_for_this_interval = volumes_for_this_interval[-75:]
        volumes_for_this_interval = np.asarray(volumes_for_this_interval)
        interval_volume_list.append(volumes_for_this_interval)
        volumes_for_this_interval = []
        # increment the timestamp and interval index
        starting_timestamp = starting_timestamp + 5.0
        interval_index = interval_index + 1
    # end of end of interval processing

    print('Processing ' + str(i + 1) + ' of ' + str(len(radar_data)) + ', interval = ' + str(interval_index))

    if isplot:
        mergedImg_path_intervaled = os.path.join(mergedImg_path, str(interval_index - 1))

        if not os.path.isdir(mergedImg_path_intervaled):
            os.mkdir(mergedImg_path_intervaled)

        closest_video_timestamp = min(videoData_timestamps,
                                      key=lambda x: abs(x - timestamp))
        closest_video_path = os.path.join(videoData_path, str(closest_video_timestamp) + '.jpg')
        closest_video_img = Image.open(closest_video_path)

        # plot the radar scatter
        ax1 = plt.subplot(2, 2, 1, projection='3d')
        ax1.set_xlim((-0.3, 0.3))
        ax1.set_ylim((-0.3, 0.3))
        ax1.set_zlim((-0.3, 0.3))
        ax1.set_xlabel('X', fontsize=10)
        ax1.set_ylabel('Y', fontsize=10)
        ax1.set_zlabel('Z', fontsize=10)
        ax1.set_title('Detected Points', fontsize=10)
        # plot the detected points
        ax1.scatter(fData['x'], fData['y'], fData['z'], c=fData['doppler'], marker='o')

    # Do DBSCAN cluster ###############
    # Do cluster ###############
    # map the points to their doppler value, this is for retrieving the doppler value after clustering
    data = np.asarray([fData['x'], fData['y'], fData['z'], fData['doppler']]).transpose()
    doppler_dict = {}
    for point in data:
        doppler_dict[tuple(point[:3])] = point[3:]
    # get rid of the doppler for clustering TODO should we consider the doppler in clustering?
    data = data[:, :3]

    db = DBSCAN(eps=DBSCAN_esp, min_samples=DBSCAN_minSamples).fit(data)
    core_samples_mask = np.zeros_like(db.labels_, dtype=bool)
    core_samples_mask[db.core_sample_indices_] = True
    labels = db.labels_
    # Number of clusters in labels, ignoring noise if present.
    n_clusters_ = len(set(labels)) - (1 if -1 in labels else 0)
    n_noise_ = list(labels).count(-1)

    if isplot:
        ax2 = plt.subplot(2, 2, 2, projection='3d')
        ax2.set_xlim((-0.3, 0.3))
        ax2.set_ylim((-0.3, 0.3))
        ax2.set_zlim((-0.3, 0.3))
        ax2.set_xlabel('X', fontsize=10)
        ax2.set_ylabel('Y', fontsize=10)
        ax2.set_zlabel('Z', fontsize=10)
        ax2.set_title('Clustered Points', fontsize=10)

    unique_labels = set(labels)
    colors = [plt.cm.Spectral(each)
              for each in np.linspace(0, 1, len(unique_labels))]

    clusters = []

    for k, col in zip(unique_labels, colors):
        if k == -1:
            # Black used for noise.
            col = [0, 0, 0, 1]
        class_member_mask = (labels == k)
        xyz = data[class_member_mask & core_samples_mask]
        if xyz.any():  # in case there are none objects
            clusters.append(xyz)  # append this cluster data to the cluster list
        # each cluster is a 3 * n matrix
        xyz = data[class_member_mask & ~core_samples_mask]
        if isplot:
            ax2.scatter(xyz[:, 0], xyz[:, 1], xyz[:, 2], 'o', c=np.array([col]), s=12, marker='X')  # plot the noise

    # find the center for each cluster
    clusters_centers = list(
        map(lambda xyz: np.array([np.mean(xyz[:, 0]), np.mean(xyz[:, 1]), np.mean(xyz[:, 2])]), clusters))
    clusters.sort(key=lambda xyz: distance.euclidean((0.0, 0.0, 0.0), np.array(
        [np.mean(xyz[:, 0]), np.mean(xyz[:, 1]), np.mean(xyz[:, 2])])))

    # plot the clusters
    for xyz, col in zip(clusters, colors):
        if isplot:
            ax2.scatter(xyz[:, 0], xyz[:, 1], xyz[:, 2], 'o', c=np.array([col]), s=28,
                        marker='o')  # plot the cluster points

    #############################
    # center normalize hand cluster
    # clear the hand cluster
    hand_cluster = []

    bbox = (0.2, 0.2, 0.2)

    if len(clusters) > 0:
        hand_cluster = clusters[0]
        point_num = hand_cluster.shape[0]

        # if the cluster is outside the 20*20*20 cm bounding box
        distance_from_center = distance.euclidean((0.0, 0.0, 0.0), np.array(
            [np.mean(hand_cluster[:, 0]), np.mean(hand_cluster[:, 1]), np.mean(hand_cluster[:, 2])]))

        if distance_from_center > distance.euclidean((0.0, 0.0, 0.0),
                                                     bbox):  # if the core of the cluster is too far away from the center
            hand_cluster = np.zeros((hand_cluster.shape[0], hand_cluster.shape[1] + 1))
        else:
            doppler_array = np.zeros((point_num, 1))
            for j in range(point_num):
                doppler_array[j:, ] = doppler_dict[tuple(hand_cluster[j, :3])]
            # append back the doppler
            hand_cluster = np.append(hand_cluster, doppler_array, 1)
            # perform column-wise min-max normalization
            # hand_minMaxScaler = MinMaxScaler()
            # hand_cluster = hand_minMaxScaler.fit_transform(hand_cluster)

    # create 3D feature space #############################
    frame_3D_volume = snapPointsToVolume(np.asarray(hand_cluster), volume_shape)
    volumes_for_this_interval.append(np.expand_dims(frame_3D_volume, axis=0))

    #############################
    # Combine the three images
    if isplot:
        plt.savefig(os.path.join(radar_3dscatter_path, str(timestamp) + '.jpg'))
        radar_3dscatter_img = Image.open(os.path.join(radar_3dscatter_path, str(timestamp) + '.jpg'))

        images = [closest_video_img, radar_3dscatter_img]  # add image here to arrange them horizontally
        widths, heights = zip(*(i.size for i in images))
        total_width = sum(widths)
        max_height = max(heights)
        new_im = Image.new('RGB', (total_width, max_height))
        x_offset = 0
        for im in images:
            new_im.paste(im, (x_offset, 0))
            x_offset += im.size[0]

        timestamp_difference = abs(float(timestamp) - float(closest_video_timestamp))
        draw = ImageDraw.Draw(new_im)

        # draw the timestamp difference on the image
        (x, y) = (20, 10)
        message = "Timestamp Difference, abs(rt-vt): " + str(timestamp_difference)
        draw.text((x, y), message, fill=white_color, font=fnt)
        # draw the timestamp
        (x, y) = (20, 30)
        message = "Timestamp: " + str(timestamp)
        draw.text((x, y), message, fill=white_color, font=fnt)

        # draw the number of points
        (x, y) = (20, 60)
        message = "Number of detected points: " + str(xyz.shape[0])
        draw.text((x, y), message, fill=white_color, font=fnt)

        # draw the number of clusters and number of noise point on the clutter plot
        (x, y) = (20, 80)
        message = "Number of clusters: " + str(n_clusters_)
        draw.text((x, y), message, fill=white_color, font=fnt)
        (x, y) = (20, 100)
        message = "Number of outliers: " + str(n_noise_)
        draw.text((x, y), message, fill=white_color, font=fnt)

        # save the combined image
        new_im.save(
            os.path.join(mergedImg_path_intervaled, str(timestamp) + '_' + str(timestamp.as_integer_ratio()[0]) +
                         '_' + str(timestamp.as_integer_ratio()[1]) + '_' + str(interval_index) + '.jpg'))
        plt.close('all')

# process the last interval ##########################################################################
if len(volumes_for_this_interval) <= 100:
    num_intervaled_samples = len(volumes_for_this_interval)
    if num_intervaled_samples < sample_per_interval / 4:
        raise Exception('Not Enough Data Points, killed')

    # decide the label
    if num_write == 1:
        if interval_index % (5 * num_write) == 1:
            this_label = 1.0
        elif interval_index % (5 * num_write) == 2:
            this_label = 4.0  # for label D
        elif interval_index % (5 * num_write) == 3:
            this_label = 12.0  # for label L
        elif interval_index % (5 * num_write) == 4:
            this_label = 13.0  # for label M
        elif interval_index % (5 * num_write) == 0:
            this_label = 16.0  # for label P
    elif num_write == 2:
        if interval_index % (5 * num_write) == 1 or interval_index % (5 * num_write) == 2:
            this_label = 1.0
        elif interval_index % (5 * num_write) == 3 or interval_index % (5 * num_write) == 4:
            this_label = 4.0  # for label D
        elif interval_index % (5 * num_write) == 5 or interval_index % (5 * num_write) == 6:
            this_label = 12.0  # for label L
        elif interval_index % (5 * num_write) == 7 or interval_index % (5 * num_write) == 8:
            this_label = 13.0  # for label M
        elif interval_index % (5 * num_write) == 9 or interval_index % (5 * num_write) == 0:
            this_label = 16.0  # for label P
    label_array.append(this_label)  # for label A

    print('Label for the last interval is ' + str(this_label) + ' Num Samples: ' + str(len(volumes_for_this_interval)))
    print('')

    # add padding, pre-padded
    if len(volumes_for_this_interval) < sample_per_interval:
        while len(volumes_for_this_interval) < sample_per_interval:
            volumes_for_this_interval.insert(0, np.expand_dims(np.zeros(volume_shape), axis=0))
    elif len(volumes_for_this_interval) > sample_per_interval:  # we take only the 75 most recent
        volumes_for_this_interval = volumes_for_this_interval[-75:]
    volumes_for_this_interval = np.asarray(volumes_for_this_interval)
    interval_volume_list.append(volumes_for_this_interval)
    volumes_for_this_interval = []
    # increment the timestamp and interval index
    starting_timestamp = starting_timestamp + 5.0
    interval_index = interval_index + 1

# start of post processing ##########################################################################
label_array = np.asarray(label_array)
interval_volume_array = np.asarray(interval_volume_list)

# validate the output shapes
assert interval_volume_array.shape == (50, 100, 1) + volume_shape
assert len(label_array) == 50

print('Saving csv and npy...')
np.save(os.path.join(out_path, 'label_array'), label_array)
np.save(os.path.join(out_path, 'intervaled_3D_volumes_' + str(volume_shape[0]) + 'x'), interval_volume_array)
print('Done!')
