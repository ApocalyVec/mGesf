import numpy as np
import pickle
import os
import shutil

import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import style

from PIL import Image, ImageDraw, ImageFont
from sklearn.cluster import DBSCAN
from sklearn.preprocessing import MinMaxScaler

from scipy.spatial import distance
# Palmpad
# # ya 0 ######################################################
# radarData_path = 'F:/palmpad/data/f_data_ya_0/f_data.p'
# videoData_path = 'F:/palmpad/data/v_data_ya_0/cam2'
# mergedImg_path = 'F:/palmpad/figures/ya_0'
# out_path = 'F:/palmpad/csv/ya_0'
#
# # ya 1 ######################################################
# radarData_path = 'F:/palmpad/data/f_data_ya_1/f_data.p'
# videoData_path = 'F:/palmpad/data/v_data_ya_1/cam2'
# mergedImg_path = 'F:/palmpad/figures/ya_1'
# out_path = 'F:/palmpad/csv/ya_1'
#
# # zl 0 ######################################################
# radarData_path = 'F:/palmpad/data/f_data_zl_0/f_data.p'
# videoData_path = 'F:/palmpad/data/v_data_zl_0/cam2'
# mergedImg_path = 'F:/palmpad/figures/zl_0'
# out_path = 'F:/palmpad/csv/zl_0'
#
# # zl 1 ######################################################
# radarData_path = 'F:/palmpad/data/f_data_zl_1/f_data.p'
# videoData_path = 'F:/palmpad/data/v_data_zl_1/cam2'
# mergedImg_path = 'F:/palmpad/figures/zl_1'
# out_path = 'F:/palmpad/csv/zl_1'
#
# # zy 0 ######################################################
# radarData_path = 'F:/palmpad/data/f_data_zy_0/f_data.p'
# videoData_path = 'F:/palmpad/data/v_data_zy_0/cam2'
# mergedImg_path = 'F:/palmpad/figures/zy_0'
# out_path = 'F:/palmpad/csv/zy_0'
# #
# # zy 1 ######################################################
# radarData_path = 'F:/palmpad/data/f_data_zy_1/f_data.p'
# videoData_path = 'F:/palmpad/data/v_data_zy_1/cam2'
# mergedImg_path = 'F:/palmpad/figures/zy_1'
# out_path = 'F:/palmpad/csv/zy_1'

# IndexPen
# ya 0 ######################################################
radarData_path = 'F:/indexPen/data/f_data_ya_0/f_data.p'
videoData_path = 'F:/indexPen/data/v_data_ya_0/cam2'
mergedImg_path = 'F:/indexPen/figures/ya_0'
out_path = 'F:/indexPen/csv/ya_0'

# ya 1 ######################################################
radarData_path = 'F:/indexPen/data/f_data_ya_1/f_data.p'
videoData_path = 'F:/indexPen/data/v_data_ya_1/cam2'
mergedImg_path = 'F:/indexPen/figures/ya_1'
out_path = 'F:/indexPen/csv/ya_1'

# # ya 2 ######################################################
radarData_path = 'F:/indexPen/data/f_data_ya_2/f_data.p'
videoData_path = 'F:/indexPen/data/v_data_ya_2/cam2'
mergedImg_path = 'F:/indexPen/figures/ya_2'
out_path = 'F:/indexPen/csv/ya_2'

# ya 3 ######################################################
radarData_path = 'F:/indexPen/data/f_data_ya_3/f_data.p'
videoData_path = 'F:/indexPen/data/v_data_ya_3/cam2'
mergedImg_path = 'F:/indexPen/figures/ya_3'
out_path = 'F:/indexPen/csv/ya_3'

# zl 0 ######################################################
radarData_path = 'F:/indexPen/data/f_data_zl_0/f_data.p'
videoData_path = 'F:/indexPen/data/v_data_zl_0/cam2'
mergedImg_path = 'F:/indexPen/figures/zl_0'
out_path = 'F:/indexPen/csv/zl_0'

# zl 1 ######################################################
radarData_path = 'F:/indexPen/data/f_data_zl_1/f_data.p'
videoData_path = 'F:/indexPen/data/v_data_zl_1/cam2'
mergedImg_path = 'F:/indexPen/figures/zl_1'
out_path = 'F:/indexPen/csv/zl_1'

# zl 2 BAD ######################################################
radarData_path = 'F:/indexPen/data/f_data_zl_2/f_data.p'
videoData_path = 'F:/indexPen/data/v_data_zl_2/cam2'
mergedImg_path = 'F:/indexPen/figures/zl_2'
out_path = 'F:/indexPen/csv/zl_2'

# zl 3 ######################################################
radarData_path = 'F:/indexPen/data/f_data_zl_3/f_data.p'
videoData_path = 'F:/indexPen/data/v_data_zl_3/cam2'
mergedImg_path = 'F:/indexPen/figures/zl_3'
out_path = 'F:/indexPen/csv/zl_3'

# # zy 0 ######################################################
# radarData_path = 'F:/indexPen/data/f_data_zy_0/f_data.p'
# videoData_path = 'F:/indexPen/data/v_data_zy_0/cam2'
# mergedImg_path = 'F:/indexPen/figures/zy_0'
# out_path = 'F:/indexPen/csv/zy_0'

# # zy 1 ######################################################
# radarData_path = 'F:/indexPen/data/f_data_zy_1/f_data.p'
# videoData_path = 'F:/indexPen/data/v_data_zy_1/cam2'
# mergedImg_path = 'F:/indexPen/figures/zy_1'
# out_path = 'F:/indexPen/csv/zy_1'

# # zy 2 ######################################################
# radarData_path = 'F:/indexPen/data/f_data_zy_2/f_data.p'
# videoData_path = 'F:/indexPen/data/v_data_zy_2/cam2'
# mergedImg_path = 'F:/indexPen/figures/zy_2'
# out_path = 'F:/indexPen/csv/zy_2'

# # zy 3 ######################################################
# radarData_path = 'F:/indexPen/data/f_data_zy_3/f_data.p'
# videoData_path = 'F:/indexPen/data/v_data_zy_3/cam2'
# mergedImg_path = 'F:/indexPen/figures/zy_3'
# out_path = 'F:/indexPen/csv/zy_3'


# utility directory to save the pyplots
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
data_for_classifier_flattened = np.zeros((len(radar_data), 4 * num_padding + 1 + 1 + 1))  # + 1 + 1 for the timestamp as integer ratio

fnt = ImageFont.truetype("arial.ttf", 16)

# Retrieve the first timestamp
starting_timestamp = radar_data[0][0]

interval_index = 1

# removed and recreate the merged image folder
if os.path.isdir(mergedImg_path):
    shutil.rmtree(mergedImg_path)
os.mkdir(mergedImg_path)

intervaled_data_list = []
intervaled_data = []

intervaled_3D_data_list = []
intervaled_3D_data = []

interval_sec = 5
sample_per_sec = 15
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
        # pad to sample_per_interval
        intervaled_data = np.asarray(intervaled_data)
        if intervaled_data.shape[0] < sample_per_interval:
            intervaled_data = np.concatenate((intervaled_data, np.zeros((sample_per_interval - intervaled_data.shape[0], num_padding*4+3))))
        elif intervaled_data.shape[0] > sample_per_interval:
            intervaled_data = intervaled_data[:sample_per_interval, :]

        if len(intervaled_3D_data) < 75:
            while(len(intervaled_3D_data) < 75):
                intervaled_3D_data.append(np.zeros((1, 20, 20, 20)))
        elif len(intervaled_3D_data) > 75:
            intervaled_3D_data = intervaled_3D_data[:75]

        intervaled_data_list.append(intervaled_data)
        intervaled_3D_data_list.append(np.asarray(intervaled_3D_data))

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
        print('Label for the last interval is ' + str(this_label) + ' Num Samples: ' + str(len(intervaled_data)))
        print('')

        # reset the interval data
        intervaled_data = []
        intervaled_3d_data = []
        starting_timestamp = starting_timestamp + 5.0
        interval_index = interval_index + 1

    mergedImg_path_intervaled = os.path.join(mergedImg_path, str(interval_index-1))

    if not os.path.isdir(mergedImg_path_intervaled):
        os.mkdir(mergedImg_path_intervaled)

    print('Processing ' + str(i + 1) + ' of ' + str(len(radar_data)) + ', interval = ' + str(interval_index))


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
        # ax.scatter(xyz[:, 0], xyz[:, 1], xyz[:, 2], 'o', c=np.array([col]), s=28, marker='o')
        if xyz.any():  # in case there are none objects
            clusters.append(xyz)  # append this cluster data to the cluster list
        # each cluster is a 3 * n matrix
        xyz = data[class_member_mask & ~core_samples_mask]
        ax2.scatter(xyz[:, 0], xyz[:, 1], xyz[:, 2], 'o', c=np.array([col]), s=12, marker='X')  # plot the noise

    # find the center for each cluster
    clusters_centers = list(
        map(lambda xyz: np.array([np.mean(xyz[:, 0]), np.mean(xyz[:, 1]), np.mean(xyz[:, 2])]), clusters))
    clusters.sort(key=lambda xyz: distance.euclidean((0.0, 0.0, 0.0), np.array(
        [np.mean(xyz[:, 0]), np.mean(xyz[:, 1]), np.mean(xyz[:, 2])])))

    # plot the clusters
    for xyz, col in zip(clusters, colors):
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
        if distance_from_center > distance.euclidean((0.0, 0.0, 0.0), bbox):
            hand_cluster = np.zeros((hand_cluster.shape[0], hand_cluster.shape[1] + 1))
        else:

            xmean = np.mean(hand_cluster[:, 0])
            xmin = np.min(hand_cluster[:, 0])
            xmax = np.max(hand_cluster[:, 0])

            ymean = np.mean(hand_cluster[:, 1])
            ymin = np.min(hand_cluster[:, 1])
            ymax = np.max(hand_cluster[:, 1])

            zmean = np.mean(hand_cluster[:, 2])
            zmin = np.min(hand_cluster[:, 2])
            zmax = np.max(hand_cluster[:, 2])

            # append back the doppler
            # doppler array for this frame
            doppler_array = np.zeros((point_num, 1))
            for j in range(point_num):
                doppler_array[j:, ] = doppler_dict[tuple(hand_cluster[j, :3])]
            # min-max normalize the velocity
            minMaxScaler = MinMaxScaler()
            doppler_array = minMaxScaler.fit_transform(doppler_array)

            hand_cluster = np.append(hand_cluster, doppler_array,
                                     1)  # TODO this part needs validation, are the put-back dopplers correct?

            # Do the Mean Normalization
            # avoid division by zero, check if all the elements in a column are the same
            if np.all(hand_cluster[:, 0][0] == hand_cluster[:, 0]) or xmin == xmax:
                hand_cluster[:, 0] = np.zeros((point_num))
            else:
                hand_cluster[:, 0] = np.asarray(list(map(lambda x: (x - xmean) / (xmax - xmin), hand_cluster[:, 0])))

            if np.all(hand_cluster[:, 1][0] == hand_cluster[:, 1]) or ymin == ymax:
                hand_cluster[:, 1] = np.zeros((point_num))
            else:
                hand_cluster[:, 1] = np.asarray(list(map(lambda y: (y - ymean) / (ymax - ymin), hand_cluster[:, 1])))

            if np.all(hand_cluster[:, 2][0] == hand_cluster[:, 2]) or zmin == zmax:
                hand_cluster[:, 2] = np.zeros((point_num))
            else:
                hand_cluster[:, 2] = np.asarray(list(map(lambda z: (z - zmean) / (zmax - zmin), hand_cluster[:, 2])))

        # pad to 50
        hand_cluster_padded = np.pad(hand_cluster, ((0, num_padding - point_num), (0, 0)), 'constant',
                                 constant_values=0)
    else:
        hand_cluster_padded = np.zeros((num_padding, 4))

    # flatten hand_cluster and add timestamp information
    hand_cluster_padded_flattened = hand_cluster_padded.reshape(( -1))
    hand_cluster_padded_flattened = np.insert(hand_cluster_padded_flattened, 0, timestamp.as_integer_ratio()[1])
    hand_cluster_padded_flattened = np.insert(hand_cluster_padded_flattened, 0, timestamp.as_integer_ratio()[0])
    hand_cluster_padded_flattened = np.insert(hand_cluster_padded_flattened, 0, timestamp)

    data_for_classifier[i] = hand_cluster_padded
    data_for_classifier_flattened[i] = hand_cluster_padded_flattened
    intervaled_data.append(hand_cluster_padded_flattened)

    # plot the normalized closest cluster
    ax3 = plt.subplot(2, 2, 3, projection='3d')
    ax3.set_xlim((-1.0, 1.0))
    ax3.set_ylim((-1.0, 1.0))
    ax3.set_zlim((-1.0, 1.0))
    ax3.set_xlabel('X', fontsize=10)
    ax3.set_ylabel('Y', fontsize=10)
    ax3.set_zlabel('Z', fontsize=10)
    ax3.set_title('CLosest Cluster', fontsize=10)


    ax3.scatter(hand_cluster_padded[:, 0], hand_cluster_padded[:, 1], hand_cluster_padded[:, 2], c=hand_cluster_padded[:, 3], marker='o')

    # create 3D feature space #############################
    hand_feature_space = np.zeros((20, 20, 20))
    for row in hand_cluster:
        feature_x = int(((row[0] + 1) * 100)/10)
        feature_y = int(((row[1] + 1) * 100)/10)
        feature_z = int(((row[2] + 1) * 100)/10)
        feature_v = row[3]

        hand_feature_space[feature_x, feature_y, feature_z] += feature_v

    # expand dimension as single channel
    hand_feature_space = np.expand_dims(hand_feature_space, axis=0)
    intervaled_3D_data.append(hand_feature_space)

    #############################
    # Combine the three images
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
    new_im.save(os.path.join(mergedImg_path_intervaled, str(timestamp) + '_' + str(timestamp.as_integer_ratio()[0]) +
                             '_' + str(timestamp.as_integer_ratio()[1]) + '_' + str(interval_index) + '.jpg'))
    plt.close('all')

# process the last interval ##########################################################################
if len(intervaled_data_list) <= 100:
    intervaled_data = np.asarray(intervaled_data)
    if intervaled_data.shape[0] < sample_per_interval:
        intervaled_data = np.concatenate(
            (intervaled_data, np.zeros((sample_per_interval - intervaled_data.shape[0], num_padding * 4 + 3))))
    elif intervaled_data.shape[0] > sample_per_interval:
        intervaled_data = intervaled_data[:sample_per_interval, :]

    if len(intervaled_3D_data) < 75:
        while (len(intervaled_3D_data) < 75):
            intervaled_3D_data.append(np.zeros(20, 20))
    elif len(intervaled_3D_data) > 75:
        intervaled_3D_data = intervaled_3D_data[:75]

    intervaled_data_list.append(intervaled_data)
    intervaled_3D_data_list.append(intervaled_3D_data)

    if num_write == 1:
        if interval_index % (5 * num_write) == 1:
            this_label = 1.0
        elif interval_index % (5 * num_write) == 2:
            this_label = 4.0  # for label D
        elif interval_index % (5 * num_write) == 3:
            this_label = 12.0  # for label L
        elif interval_index % (5 * num_write) == 4:
            this_label = 13.0  # for label M
        elif interval_index % (5 * num_write) == 5:
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
    print('Label for the last interval is ' + str(this_label))

# start of post processing ##########################################################################

import pandas as pd

data_for_classifier_flattened = pd.DataFrame(data_for_classifier_flattened)
intervaled_data_list = np.asarray(intervaled_data_list)
label_array = np.asarray(label_array)
# remove the timestamp for the classifier

intervaled_data_ts_removed = []
for i, i_data in enumerate(intervaled_data_list):
    intervaled_data_ts_removed.append(i_data[:, 3:])
intervaled_data_ts_removed = np.asarray(intervaled_data_ts_removed)

intervaled_3D_data_array = np.asarray(intervaled_3D_data_list)
print('Saving csv and npy...')
# data_for_classifier_flattened.to_csv(os.path.join(out_path, 'flattened.csv'))
# np.save(os.path.join(out_path, 'intervaled'), intervaled_data_list)

# np.save(os.path.join(out_path, 'intervaled_ts_removed'), intervaled_data_ts_removed)
# np.save(os.path.join(out_path, 'label_array'), label_array)
# np.save(os.path.join(out_path, 'intervaled_3D'), intervaled_3D_data_array)

print('Done!')

# data_for_classifier_flattened.to_csv('F:/config_detection/csv/*.csv')

