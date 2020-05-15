import numpy as np
import pickle
import os

import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import style

from PIL import Image, ImageDraw, ImageFont
from sklearn.cluster import DBSCAN

from scipy.spatial import distance

# OnNotOn #######################################################################################################
# zl path
# radarData_path = '/Users/hanfei/data/072819_zl_onNotOn/f_data-2019-07-28_22-11-01.258054_zl_onNotOn_rnn/f_data.p'
# videoData_path = '/Users/hanfei/data/072819_zl_onNotOn/v_data-2019-07-28_22-10-32.249041_zl_onNotOn_rnn/cam1'
# mergedImg_path = '/Users/hanfei/figures/new'
# raw_path = 'F:/onNotOn_raw/zl_onNoton_raw.p'

# ag path
# radarData_path = '/Users/hanfei/data/072819_ag_onNotOn/f_data-2019-07-28_21-44-17.102820_ag_onNotOn_rnn/f_data.p'
# videoData_path = '/Users/hanfei/data/072819_ag_onNotOn/v_data-2019-07-28_21-44-08.514321_ag_onNotOn_rnn/cam1'
# mergedImg_path = '/Users/hanfei/figures/ag_onNotOn_x03y03z03_clustered_esp02ms4'
# raw_path = 'F:/onNotOn_raw/ag_onNoton_raw.p'

# zy path
# radarData_path = '/Users/hanfei/data/072919_zy_onNotOn/f_data.p'
# videoData_path = '/Users/hanfei/data/072919_zy_onNotOn/v_data-2019-07-29_11-40-34.810544_zy_onNotOn/cam1'
# mergedImg_path = '/Users/hanfei/figures/zy_onNotOn_x03y03z03_clustered_esp02ms4'
# raw_path = 'F:/onNotOn_raw/zy_onNoton_raw.p'

# Palmpad Test ####################################################################################################


# utility directory to save the pyplots
radar_3dscatter_path = '/Users/hanfei/figures/utils/radar_3dscatter'

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
num_padding = 50
data_for_classifier = np.zeros((len(radar_data), num_padding, 4))

fnt = ImageFont.truetype("Arial.ttf", 16)

for i, radarFrame in enumerate(radar_data):

    timestamp, fData = radarFrame
    print('Processing ' + str(i + 1) + ' of ' + str(len(radar_data)))

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

    # get rid of the doppler for clustering TODO should we consider the doppler in clustering?
    doppler_dict = {}
    for point in data:
        doppler_dict[tuple(point[:3])] = point[3:]
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

    if len(clusters) > 0:
        hand_cluster = clusters[0]

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
        point_num = hand_cluster.shape[0]

        doppler_array = np.zeros((point_num, 1))
        for j in range(point_num):
            doppler_array[j:, ] = doppler_dict[tuple(hand_cluster[j, :3])]
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

    data_for_classifier[i] = hand_cluster_padded
    # plot the normalized closest cluster
    ax3 = plt.subplot(2, 2, 3, projection='3d')
    ax3.set_xlim((-1.0, 1.0))
    ax3.set_ylim((-1.0, 1.0))
    ax3.set_zlim((-1.0, 1.0))
    ax3.set_xlabel('X', fontsize=10)
    ax3.set_ylabel('Y', fontsize=10)
    ax3.set_zlabel('Z', fontsize=10)
    ax3.set_title('Closest Cluster', fontsize=10)


    ax3.scatter(hand_cluster_padded[:, 0], hand_cluster_padded[:, 1], hand_cluster_padded[:, 2], c=hand_cluster_padded[:, 3], marker='o')

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
    new_im.save(os.path.join(mergedImg_path, str(timestamp) + '_' + str(timestamp.as_integer_ratio()[0]) + '_' + str(timestamp.as_integer_ratio()[1] + '.jpg')))
    plt.close('all')

# Following Code is for labeling ##################################

# label_radar_data = []
#
# (x, y) = (50, 30)
#
# for i in range(len(radar_data)):
#     timestamp = radar_data[i][0]
#     data = radar_data[i][1]
#
#     print('Radar Image ' + str(i + 1) + ' of ' + str(len(radar_data)) + '   : ' + str(timestamp), end='')
#
#     im = Image.open(os.path.join(mergedImg_path, str(timestamp) + '.jpg'))
#     draw = ImageDraw.Draw(im)
#     message = 'Current Image'
#     white_color = 'rgb(255, 255, 255)'
#     draw.text((x, y), message, fill=white_color)
#     im.show()
#
#     im = Image.open(os.path.join(mergedImg_path, str(radar_data[i+1][0]) + '.jpg'))
#     draw = ImageDraw.Draw(im)
#     message = 'Previous Image'
#     white_color = 'rgb(255, 0, 255)'
#     draw.text((x, y), message, fill=white_color)
#     im.show()
#
#     im = Image.open(os.path.join(mergedImg_path, str(radar_data[i-1][0]) + '.jpg'))
#     draw = ImageDraw.Draw(im)
#     message = 'Next Image'
#     white_color = 'rgb(0, 255, 255)'
#     draw.text((x, y), message, fill=white_color)
#     im.show()
#
#     while 1:
#         label = int(input(' Label 0 for unchanged, 1 for lifting, and 2 for setting'))
#         if label == 1 or label == 2 or label == 0:
#             break
#         else:
#             print('Label must be 0, 1 or 2; your input is ' + str(label))
#
#     label_radar_data.append((timestamp, data, label))
