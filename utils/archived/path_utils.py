import random
from itertools import product

import numpy as np
import math

import pickle
import os
import shutil

import matplotlib.pyplot as plt
from matplotlib import style
from mpl_toolkits.mplot3d import axes3d, Axes3D #<-- Note the capitalization!

from PIL import Image, ImageDraw, ImageFont
from sklearn.cluster import DBSCAN

from scipy.spatial import distance
import pandas as pd

from sklearn.preprocessing import MinMaxScaler

from utils.data_utils import produce_voxel, snapPointsToVolume, parse_deltalize_recording, merge_dict
from utils.transformation import translate, sphere_search, rotateZ, rotateY, rotateX, scale


augmentation_list = ['trans', 'rot', 'scale', 'clipping']

# variables used in snapPointsToVolume
xmin, xmax = -0.5, 0.5
ymin, ymax = 0.0, 0.5
zmin, zmax = -0.5, 0.5

heatMin, heatMax = -1.0, 1.0
xyzScaler = MinMaxScaler().fit(np.array([[xmin, ymin, zmin],
                                         [xmax, ymax, zmax]]))
heatScaler = MinMaxScaler().fit(np.array([[heatMin],
                                          [heatMax]]))

# volumn.shape = (5, 5, 5)

def midpoints(x):
    sl = ()
    for i in range(x.ndim):
        x = (x[sl + np.index_exp[:-1]] + x[sl + np.index_exp[1:]]) / 2.0
        sl += np.index_exp[:]
    return x

def idp_preprocess(paths, is_plot=False, augmentation=(),
                   seeds=np.random.normal(0, 0.02, 5000), util_path='E:/temp'):
    # utility directory to save the pyplots
    radar_points_data_path, radar_voxel_data_path, videoData_path, figure_path, out_path, identity_string = paths

    radar_points = pickle.load(open(radar_points_data_path, 'rb'))
    radar_voxel = pickle.load(open(radar_voxel_data_path, 'rb'))

    video_frame_list = os.listdir(videoData_path)
    video_frame_timestamps = list(map(lambda x: float(x.strip('.jpg')), video_frame_list))

    samples_per_session = [20, 40]
    interval_duration = 4.0
    sample_per_sec = 20

    style.use('fivethirtyeight')
    white_color = 'rgb(255, 255, 255)'
    black_color = 'rgb(0, 0, 0)'
    red_color = 'rgb(255, 0, 0)'

    DBSCAN_esp = 0.2
    DBSCAN_minSamples = 3

    # input data for the classifier that has the shape n*4*100, n being the number of samples

    fnt = ImageFont.truetype("arial.ttf", 16)

    # Retrieve the first timestamp
    assert [x[0] for x in radar_points] == [x[0] for x in radar_voxel]

    starting_timestamp = radar_points[0][0]
    interval_index = 0

    # removed and recreate the merged image folder
    if is_plot:
        if os.path.isdir(figure_path):
            shutil.rmtree(figure_path)
        os.mkdir(figure_path)

    volume_shape = (25, 25, 25)

    interval_voxel_list = []
    this_voxel_list = []

    sample_per_interval = int(interval_duration * sample_per_sec)

    aug_string = ''
    if augmentation:
        print('Use augmentation: ' + str(augmentation))
        for aug in augmentation:
            aug_string += '_' + aug
    else:
        print('No augmentation applied')

    print('Label Cheat-sheet:')
    print('0 for 0')
    print('1 for 1')
    print('2 for 2')
    print('3 for 3')
    print('4 for 4')
    print('5 for 5')
    print('6 for 6')
    print('7 for 7')
    print('8 for 8')
    print('9 for 9')

    label_array = []
    this_label = 0

    for i, (this_points_and_ts, this_voxel_and_ts) in enumerate(zip(radar_points, radar_voxel)):

        # retrieve the timestamp making sure the data is synced
        assert this_points_and_ts[0] == this_voxel_and_ts[0]
        this_timestamp = this_points_and_ts[0]
        this_points = this_points_and_ts[1]
        this_voxel = this_voxel_and_ts[1]
        print('Processing ' + str(i + 1) + ' of ' + str(len(radar_points)) + ', interval = ' + str(interval_index))

        if is_plot:
            figure_intervaled_path = os.path.join(figure_path, str(interval_index))

            if not os.path.isdir(figure_intervaled_path):
                os.mkdir(figure_intervaled_path)

            closest_video_timestamp = min(video_frame_timestamps,
                                          key=lambda x: abs(x - this_timestamp))
            closest_video_path = os.path.join(videoData_path, str(closest_video_timestamp) + '.jpg')
            closest_video_img = Image.open(closest_video_path)

            # plot the radar scatter
            # ax1 = plt.subplot(2, 2, 1, projection='3d')
            # ax1.set_xlim((-0.3, 0.3))
            # ax1.set_ylim((-0.3, 0.3))
            # ax1.set_zlim((-0.3, 0.3))
            # ax1.set_xlabel('X', fontsize=10)
            # ax1.set_ylabel('Y', fontsize=10)
            # ax1.set_zlabel('Z', fontsize=10)
            # ax1.set_title('Detected Points', fontsize=10)
            # # plot the detected points
            # ax1.scatter(this_points[:, 0], this_points[:, 1], this_points[:, 2], c=this_points[:, 3], marker='o')

        # assert np.all(produce_voxel(this_points) == this_voxel)

        # apply augmentation to hand cluster #############################
        if len(this_points) > 0:
            if 'trans' in augmentation:
                for p in np.nditer(this_points[:, :3], op_flags=['readwrite']):
                    p[...] = p + random.choice(seeds)
            if 'rot' in augmentation:
                this_points[:, :3] = rotateX(this_points[:, :3], 720 * random.choice(seeds))
                this_points[:, :3] = rotateY(this_points[:, :3], 720 * random.choice(seeds))
                this_points[:, :3] = rotateZ(this_points[:, :3], 720 * random.choice(seeds))
            if 'scale' in augmentation:
                s = 1 + random.choice(seeds)
                this_points[:, :3] = scale(this_points[:, :3], x=s, y=s, z=s)

        if is_plot:
            # ax3 = plt.subplot(2, 2, 3, projection='3d')
            ax3 = plt.subplot(111, projection='3d')

            ax3.set_xlim((-0.3, 0.3))
            ax3.set_ylim((-0.3, 0.3))
            ax3.set_zlim((-0.3, 0.3))
            ax3.set_xlabel('X', fontsize=10)
            ax3.set_ylabel('Y', fontsize=10)
            ax3.set_zlabel('Z', fontsize=10)
            ax3.set_title('Detected Points', fontsize=20)

            ax3.scatter(this_points[:, 0], this_points[:, 1], this_points[:, 2], ',', c=this_points[:, 3], s=28,
                        marker='o')

        # create 3D feature space #############################
        produced_voxel = produce_voxel(this_points, isClipping='clp' in augmentation)


        this_voxel_list.append(produced_voxel)
        # Plot the hand cluster #########################################
        # Combine the three images
        if is_plot:
            # plot the voxel
            # ax4 = plt.subplot(2, 2, 4, projection='3d')
            # ax4.set_aspect('equal')
            # ax4.set_xlabel('X', fontsize=10)
            # ax4.set_ylabel('Y', fontsize=10)
            # ax4.set_zlabel('Z', fontsize=10)
            # ax4.set_title('voxel', fontsize=10)
            # ax4.voxels(produced_voxel[0])

            plt.savefig(os.path.join(util_path, str(this_timestamp) + '.jpg'))
            radar_3dscatter_img = Image.open(os.path.join(util_path, str(this_timestamp) + '.jpg'))

            images = [closest_video_img, radar_3dscatter_img]  # add image here to arrange them horizontally
            widths, heights = zip(*(i.size for i in images))
            total_width = sum(widths)
            max_height = max(heights)
            new_im = Image.new('RGB', (total_width, max_height))
            x_offset = 0
            for im in images:
                new_im.paste(im, (x_offset, 0))
                x_offset += im.size[0]

            if False:
                timestamp_difference = abs(float(this_timestamp) - float(closest_video_timestamp))
                draw = ImageDraw.Draw(new_im)

                # draw the timestamp difference on the image
                (x, y) = (20, 10)
                message = "Timestamp Difference, abs(rt-vt): " + str(timestamp_difference)
                draw.text((x, y), message, fill=white_color, font=fnt)
                # draw the timestamp
                (x, y) = (20, 30)
                message = "Timestamp: " + str(this_timestamp)
                draw.text((x, y), message, fill=white_color, font=fnt)

                # draw the number of points
                (x, y) = (20, 60)
                message = "Number of detected points: " + str(this_points.shape[0])
                draw.text((x, y), message, fill=white_color, font=fnt)

            # save the combined image
            new_im.save(
                os.path.join(figure_intervaled_path, str(this_timestamp) + '_' + str(this_timestamp.as_integer_ratio()[0]) +
                             '_' + str(this_timestamp.as_integer_ratio()[1]) + '_' + str(interval_index) + '.jpg'))
            plt.close('all')

        # calculate the interval ############################
        if (this_timestamp - starting_timestamp) >= interval_duration or i == len(radar_voxel)-1:
            # increment the timestamp and interval index
            starting_timestamp = starting_timestamp + interval_duration
            this_label = math.floor(interval_index / 2)
            interval_index = interval_index + 1

            # decide the label
            # if interval_index % inter_arg == 1 or interval_index % inter_arg == 2:
            #     this_label = 0  # for label DEL
            # elif interval_index % inter_arg == 3 or interval_index % inter_arg == 4:
            #     this_label = 1  # for label D
            # elif interval_index % inter_arg == 5 or interval_index % inter_arg == 6:
            #     this_label = 2  # for label E
            # elif interval_index % inter_arg == 7 or interval_index % inter_arg == 8:
            #     this_label = 3  # for label H
            # elif interval_index % inter_arg == 9 or interval_index % inter_arg == 0:
            #     this_label = 4  # for label L
            # elif interval_index % inter_arg == 11 or interval_index % inter_arg == 12:
            #     this_label = 5  # for label O
            # elif interval_index % inter_arg == 13 or interval_index % inter_arg == 14:
            #     this_label = 6  # for label R
            # elif interval_index % inter_arg == 15 or interval_index % inter_arg == 16:
            #     this_label = 7  # for label W
            # elif interval_index % inter_arg == 17 or interval_index % inter_arg == 18:
            #     this_label = 8  # for label SPC
            # elif interval_index % inter_arg == 19 or interval_index % inter_arg == 0:
            #     this_label = 9  # for label EXC
            label_array.append(this_label)

            print('Interval' + str(interval_index) + ': Label-' + str(this_label) + ' # of Samples- ' + str(len(this_voxel_list)))
            print('')

            # add padding, pre-padded
            if len(this_voxel_list) < sample_per_interval:
                while len(this_voxel_list) < sample_per_interval:
                    this_voxel_list.insert(0, np.expand_dims(np.zeros(volume_shape), axis=0))
            elif len(this_voxel_list) > sample_per_interval:  # we take only the most recent timesteps
                this_voxel_list = this_voxel_list[-sample_per_interval:]
            this_voxel_list = np.asarray(this_voxel_list)
            interval_voxel_list.append(this_voxel_list)
            this_voxel_list = []

        # end of end of interval processing

    # start of post processing ##########################################################################
    sps = min(samples_per_session, key=lambda x:abs(x-len(label_array)))

    label_array = np.asarray(label_array)[:sps]
    interval_volume_array = np.asarray(interval_voxel_list)[:sps]

    assert len(interval_volume_array) == len(label_array) and len(label_array) in samples_per_session

    interval_mean = np.mean(interval_volume_array)
    print('Interval mean is ' + str(interval_mean))
    assert interval_mean < 1.0

    # validate the output shapes

    dataset_path = 'F:/alldataset/idp_dataset'
    label_dict_path = 'F:/alldataset/idp_label_dict.p'

    print('Saving chunks to ' + dataset_path + '...')

    # load label dict
    if os.path.exists(label_dict_path):
        label_dict = pickle.load(open(label_dict_path, 'rb'))
    else:  # create anew if does not exist
        label_dict = {}

    # put the label into the dict and save data
    for i, l_and_d in enumerate(zip(label_array, interval_volume_array)):
        print('Saving chunk #' + str(i))

        label_dict[identity_string + '_' + str(i) + aug_string] = l_and_d[0]
        np.save(os.path.join(dataset_path, identity_string + '_' + str(i) + aug_string), l_and_d[1])

    # save label dict to disk
    pickle.dump(label_dict, open(label_dict_path, 'wb'))

    print('Current number of labels is ' + str(len(label_dict)))
    print('Done saving to ' + dataset_path)


def generate_path(subject_name: str, case_index: int, mode: str) -> tuple:

    identity_string = subject_name + '_' + str(case_index)
    f_dir = 'f_data_' + mode + '_' + identity_string
    v_dir = 'v_data_' + mode + '_' + identity_string

    root_path = 'E:/alldata_' + mode

    radar_point_data_path = os.path.join(root_path, f_dir, 'f_data_points.p')
    radar_voxel_data_path = os.path.join(root_path, f_dir, 'f_data_voxel.p')

    videoData_path = os.path.join(root_path, v_dir, 'cam2')
    mergedImg_path = os.path.join('E:/allfig_' + mode, identity_string)
    out_path = os.path.join('E:/alldataset', mode + '_' + identity_string)

    return radar_point_data_path, radar_voxel_data_path, videoData_path, mergedImg_path, out_path, identity_string


def generate_path_thm_leap(subject_name: str, case_index: int, date: int) -> tuple:

    identity_string = subject_name + '_' + str(case_index)
    f_dir = 'f_data_thm_' + identity_string
    v_file = 'recording_' + identity_string + '.txt'

    f_data_root = 'D:/data_thm_leap_' + str(date) +'/data'
    recording_data_root = 'D:/data_thm_leap_' + str(date) +'/recordings'

    radar_point_data_path = os.path.join(f_data_root, f_dir, 'f_data_points.p')
    radar_voxel_data_path = os.path.join(f_data_root, f_dir, 'f_data_voxel.p')

    recording_path = os.path.join(recording_data_root, v_file)

    assert os.path.exists(radar_point_data_path)
    assert os.path.exists(radar_voxel_data_path)
    assert os.path.exists(recording_path)

    return radar_point_data_path, radar_voxel_data_path, recording_path, identity_string


def generate_train_val_ids(test_ratio, dataset):

    data_ids = list(map(lambda x: os.path.splitext(x)[0], dataset))

    # use pre-set random for reproducibility
    random.seed(3)
    random.shuffle(data_ids)

    num_data = len(data_ids)
    line = int((1-test_ratio) * num_data)
    train_ids = data_ids[:line]
    test_ids = data_ids[line:]

    data_dict = {'train': [], 'validation': []}

    for train_sample in train_ids:
        data_dict['train'].append(train_sample)

    for test_sample in test_ids:
        data_dict['validation'].append((test_sample))

    return data_dict


def thm_preprocess(paths, is_plot=False, augmentation=(),
                   seeds=np.random.normal(0, 0.02, 5000), util_path='E:/temp', buffer_size=1):
    # utility directory to save the pyplots
    radar_points_data_path, radar_voxel_data_path, videoData_path, figure_path, out_path, identity_string = paths

    radar_points = pickle.load(open(radar_points_data_path, 'rb'))
    radar_voxel = pickle.load(open(radar_voxel_data_path, 'rb'))

    video_frame_list = os.listdir(videoData_path)
    video_frame_timestamps = list(map(lambda x: float(x.strip('.jpg')), video_frame_list))

    dataset_path = 'D:/alldataset/thm_dataset_ts_5/data'

    style.use('fivethirtyeight')
    white_color = 'rgb(255, 255, 255)'

    # input data for the classifier that has the shape n*4*100, n being the number of samples
    fnt = ImageFont.truetype("arial.ttf", 16)

    # Retrieve the first timestamp
    assert [x[0] for x in radar_points] == [x[0] for x in radar_voxel]

    starting_timestamp = radar_points[0][0]
    interval_index = 0

    # removed and recreate the merged image folder
    if is_plot:
        if os.path.isdir(figure_path):
            shutil.rmtree(figure_path)
        os.mkdir(figure_path)

    volume_shape = (25, 25, 25)

    this_voxel_list = []

    aug_string = ''
    if augmentation:
        print('Use augmentation: ' + str(augmentation))
        for aug in augmentation:
            aug_string += '_' + aug
    else:
        print('No augmentation applied')

    buffer = []

    for i, (this_points_and_ts, this_voxel_and_ts) in enumerate(zip(radar_points, radar_voxel)):
        # retrieve the timestamp making sure the data is synced
        assert this_points_and_ts[0] == this_voxel_and_ts[0]
        this_timestamp = this_points_and_ts[0]
        this_points = this_points_and_ts[1]
        this_voxel = this_voxel_and_ts[1]
        print('Processing ' + str(i + 1) + ' of ' + str(len(radar_points)) + ', items in buffer = ' + str(len(buffer)))

        if is_plot:
            figure_intervaled_path = os.path.join(figure_path, str(interval_index - 1))

            if not os.path.isdir(figure_intervaled_path):
                os.mkdir(figure_intervaled_path)

            closest_video_timestamp = min(video_frame_timestamps,
                                          key=lambda x: abs(x - this_timestamp))
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
            ax1.scatter(this_points[:, 0], this_points[:, 1], this_points[:, 2], c=this_points[:, 3], marker='o')

        # assert np.all(produce_voxel(this_points) == this_voxel)

        # apply augmentation to hand cluster #############################
        if len(this_points) > 0:
            if 'trans' in augmentation:
                for p in np.nditer(this_points[:, :3], op_flags=['readwrite']):
                    p[...] = p + random.choice(seeds)
            if 'rot' in augmentation:
                this_points[:, :3] = rotateX(this_points[:, :3], 720 * random.choice(seeds))
                this_points[:, :3] = rotateY(this_points[:, :3], 720 * random.choice(seeds))
                this_points[:, :3] = rotateZ(this_points[:, :3], 720 * random.choice(seeds))
            if 'scale' in augmentation:
                s = 1 + random.choice(seeds)
                this_points[:, :3] = scale(this_points[:, :3], x=s, y=s, z=s)

        if is_plot:
            ax3 = plt.subplot(2, 2, 3, projection='3d')
            ax3.set_xlim((-0.3, 0.3))
            ax3.set_ylim((-0.3, 0.3))
            ax3.set_zlim((-0.3, 0.3))
            ax3.set_xlabel('X', fontsize=10)
            ax3.set_ylabel('Y', fontsize=10)
            ax3.set_zlabel('Z', fontsize=10)
            ax3.set_title('Hand Cluster', fontsize=10)

            ax3.scatter(this_points[:, 0], this_points[:, 1], this_points[:, 2], 'o', c=this_points[:, 3], s=28,
                        marker='o')

        # create 3D feature space #############################
        if 'clp' in augmentation:
            produced_voxel = produce_voxel(this_points, isClipping=True)
        else:
            produced_voxel = produce_voxel(this_points, isClipping=False)

        if buffer_size == 1:
            this_path = os.path.join(dataset_path, str(this_timestamp.as_integer_ratio()[0]) + '_' + str(
                this_timestamp.as_integer_ratio()[1]) + aug_string)
            if os.path.exists(this_path):
                raise Exception('File ' + this_path + ' already exists. THIS SHOULD NEVER HAPPEN!')
            np.save(this_path, np.asarray(produced_voxel))  # just save the voxel
            print('saved to ' + this_path)
        else:
            if len(buffer) == buffer_size:
                buffer = buffer[-buffer_size + 1:]
                buffer.append(produced_voxel)

                # print('saving npy...', end='')
                this_path = os.path.join(dataset_path, str(this_timestamp.as_integer_ratio()[0]) + '_' + str(
                    this_timestamp.as_integer_ratio()[1]) + aug_string)
                if os.path.exists(this_path):
                    raise Exception('File ' + this_path + ' already exists. THIS SHOULD NEVER HAPPEN!')
                np.save(this_path, np.asarray(buffer))
                print('saved to ' + this_path)

            else:
                buffer.append(produced_voxel)

        # Plot the hand cluster #########################################
        # Combine the three images
        if is_plot:
            # plot the voxel
            ax4 = plt.subplot(2, 2, 4, projection='3d')
            ax4.set_aspect('equal')
            ax4.set_xlabel('X', fontsize=10)
            ax4.set_ylabel('Y', fontsize=10)
            ax4.set_zlabel('Z', fontsize=10)
            ax4.set_title('voxel', fontsize=10)
            ax4.voxels(produced_voxel[0])

            plt.savefig(os.path.join(util_path, str(this_timestamp) + '.jpg'))
            radar_3dscatter_img = Image.open(os.path.join(util_path, str(this_timestamp) + '.jpg'))

            images = [closest_video_img, radar_3dscatter_img]  # add image here to arrange them horizontally
            widths, heights = zip(*(i.size for i in images))
            total_width = sum(widths)
            max_height = max(heights)
            new_im = Image.new('RGB', (total_width, max_height))
            x_offset = 0
            for im in images:
                new_im.paste(im, (x_offset, 0))
                x_offset += im.size[0]

            timestamp_difference = abs(float(this_timestamp) - float(closest_video_timestamp))
            draw = ImageDraw.Draw(new_im)

            # draw the timestamp difference on the image
            (x, y) = (20, 10)
            message = "Timestamp Difference, abs(rt-vt): " + str(timestamp_difference)
            draw.text((x, y), message, fill=white_color, font=fnt)
            # draw the timestamp
            (x, y) = (20, 30)
            message = "Timestamp: " + str(this_timestamp)
            draw.text((x, y), message, fill=white_color, font=fnt)

            # draw the number of points
            (x, y) = (20, 60)
            message = "Number of detected points: " + str(this_points.shape[0])
            draw.text((x, y), message, fill=white_color, font=fnt)

            # save the combined image
            new_im.save(
                os.path.join(figure_intervaled_path, str(this_timestamp) + '_' + str(this_timestamp.as_integer_ratio()[0]) +
                             '_' + str(this_timestamp.as_integer_ratio()[1]) + '_' + str(interval_index) + '.jpg'))
            plt.close('all')

    # validate the output shapes


def thm_leap_preprocess(paths, dataset_path, label_path, is_plot=False, augmentation=(),
                   seeds=np.random.normal(0, 0.02, 5000), buffer_size=1):
    # utility directory to save the pyplots
    radar_points_data_path, radar_voxel_data_path, recording_path, identity_string= paths

    radar_points = pickle.load(open(radar_points_data_path, 'rb'))
    radar_voxel = pickle.load(open(radar_voxel_data_path, 'rb'))
    # Retrieve the first timestamp
    assert [x[0] for x in radar_points] == [x[0] for x in radar_voxel]

    interval_index = 0

    aug_string = ''
    if augmentation:
        print('Use augmentation: ' + str(augmentation))
        for aug in augmentation:
            aug_string += '_' + aug
    else:
        print('No augmentation applied')

    # process recordings
    recording_dict = parse_deltalize_recording(recording_path)[0]
    recording_timestamps = list(recording_dict.keys())

    labels = dict()
    buffer = []
    timestamp_diff_list = []
    for i, (this_ts_points, this_ts_voxel) in enumerate(zip(radar_points, radar_voxel)):
        # retrieve the timestamp making sure the data is synced
        assert this_ts_points[0] == this_ts_voxel[0]
        this_timestamp = this_ts_points[0]
        this_points = this_ts_points[1]
        this_voxel = this_ts_voxel[1]
        this_identifier = str(this_timestamp.as_integer_ratio()[0]) + '_' + str(
                    this_timestamp.as_integer_ratio()[1]) + aug_string
        this_path = os.path.join(dataset_path, this_identifier)
        if os.path.exists(this_path):
            raise Exception('File ' + this_path + ' already exists. THIS SHOULD NEVER HAPPEN!')
        print('Processing ' + str(i + 1) + ' of ' + str(len(radar_points)) + ', items in buffer = ' + str(len(buffer)))

        # find label
        closest_recording_timestamp = min(recording_timestamps, key=lambda x: abs(x - this_timestamp))
        timestamp_diff_list.append(closest_recording_timestamp - this_timestamp)

        # apply augmentation to hand cluster #############################
        if len(this_points) > 0:
            if 'trans' in augmentation:
                for p in np.nditer(this_points[:, :3], op_flags=['readwrite']):
                    p[...] = p + random.choice(seeds)
            if 'rot' in augmentation:
                this_points[:, :3] = rotateX(this_points[:, :3], 720 * random.choice(seeds))
                this_points[:, :3] = rotateY(this_points[:, :3], 720 * random.choice(seeds))
                this_points[:, :3] = rotateZ(this_points[:, :3], 720 * random.choice(seeds))
            if 'scale' in augmentation:
                s = 1 + random.choice(seeds)
                this_points[:, :3] = scale(this_points[:, :3], x=s, y=s, z=s)

        # create 3D feature space #############################
        if 'clp' in augmentation:
            produced_voxel = produce_voxel(this_points, isClipping=True)
        else:
            produced_voxel = this_voxel

        if buffer_size == 1:
            np.save(this_path, np.asarray(produced_voxel))  # just save the voxel
            # mark the location of the finger for this radar frame
            labels[this_identifier] = (recording_dict[closest_recording_timestamp])
            print('saved to ' + this_path)
        else:
            if len(buffer) == buffer_size:
                buffer = buffer[-buffer_size + 1:]
                buffer.append(produced_voxel)
                np.save(this_path, np.asarray(buffer))
                # mark the location of the finger for this radar frame
                labels[this_identifier] = (recording_dict[closest_recording_timestamp])
                print('saved to ' + this_path)
            else:
                buffer.append(produced_voxel)

    # make sure that the recording timestamp is not far from that of the radar's
    assert np.mean(timestamp_diff_list) < 0.001

    # load label dict
    if os.path.exists(label_path):
        labels_existing = pickle.load(open(label_path, 'rb'))
        labels = merge_dict([labels_existing, labels])
    print('Number of items in the label dict is ' + str(len(labels)))
    pickle.dump(labels, open(label_path, 'wb'))
