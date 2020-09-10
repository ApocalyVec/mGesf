import datetime

from sklearn.model_selection import train_test_split
from sklearn.preprocessing import OneHotEncoder
import numpy as np

from tensorflow.python.keras.callbacks import EarlyStopping, ModelCheckpoint

import matplotlib.pyplot as plt
import tensorflow as tf
import pickle
import time
import warnings
from collections import OrderedDict

import numpy as np
import os


import tensorflow as tf
from tensorflow.python.keras import Sequential, Model
from tensorflow.python.keras.layers import TimeDistributed, Conv2D, BatchNormalization, MaxPooling2D, Flatten, \
    concatenate, LSTM, Dropout, Dense

import os
import pickle

import numpy as np
import pandas as pd
from scipy.spatial import distance
from sklearn.cluster import DBSCAN
from sklearn.metrics import confusion_matrix
import matplotlib.pyplot as plt

import time

rd_shape = (8, 16)
ra_shape = (8, 64)


def make_model_no_reg(classes, points_per_sample, channel_mode='channels_last'):
    # creates the Time Distributed CNN for range Doppler heatmap ##########################
    mmw_rdpl_input = (int(points_per_sample),) + rd_shape + (1,) if channel_mode == 'channels_last' else (points_per_sample, 1) + rd_shape
    mmw_rdpl_TDCNN = Sequential()
    mmw_rdpl_TDCNN.add(
        TimeDistributed(
            Conv2D(filters=8, kernel_size=(3, 3), data_format=channel_mode,
                  #  kernel_regularizer=tf.keras.regularizers.l2(l=1e-5),
                  #  bias_regularizer=tf.keras.regularizers.l2(l=1e-5),
                  #  activity_regularizer=tf.keras.regularizers.l2(l=1e-5),
                   kernel_initializer='random_uniform'),
            input_shape=mmw_rdpl_input))
    mmw_rdpl_TDCNN.add(TimeDistributed(tf.keras.layers.LeakyReLU(alpha=0.1)))
    mmw_rdpl_TDCNN.add(TimeDistributed(BatchNormalization()))
    mmw_rdpl_TDCNN.add(TimeDistributed(
        Conv2D(filters=16, kernel_size=(3, 3),
              #  kernel_regularizer=tf.keras.regularizers.l2(l=1e-5),
              #  bias_regularizer=tf.keras.regularizers.l2(l=1e-5),
              #  activity_regularizer=tf.keras.regularizers.l2(l=1e-5)
               )))
    mmw_rdpl_TDCNN.add(TimeDistributed(tf.keras.layers.LeakyReLU(alpha=0.1)))
    mmw_rdpl_TDCNN.add(TimeDistributed(BatchNormalization()))
    mmw_rdpl_TDCNN.add(TimeDistributed(MaxPooling2D(pool_size=2)))
    # mmw_rdpl_TDCNN.add(TimeDistributed(
    #     Conv2D(filters=32, kernel_size=(3, 3),
    #            kernel_regularizer=tf.keras.regularizers.l2(l=0.01),
    #            bias_regularizer=tf.keras.regularizers.l2(l=0.01))))
    # mmw_rdpl_TDCNN.add(TimeDistributed(tf.keras.layers.LeakyReLU(alpha=0.1)))
    # mmw_rdpl_TDCNN.add(TimeDistributed(BatchNormalization()))
    # mmw_rdpl_TDCNN.add(TimeDistributed(MaxPooling2D(pool_size=2)))
    mmw_rdpl_TDCNN.add(TimeDistributed(Flatten()))  # this should be where layers meets

    # creates the Time Distributed CNN for range Azimuth heatmap ###########################
    mmw_razi_input = (int(points_per_sample),) + ra_shape + (1,) if channel_mode == 'channels_last' else (points_per_sample, 1) + ra_shape
    mmw_razi_TDCNN = Sequential()
    mmw_razi_TDCNN.add(
        TimeDistributed(
            Conv2D(filters=8, kernel_size=(3, 3),
                  #  kernel_regularizer=tf.keras.regularizers.l2(l=1e-5),
                  #  bias_regularizer=tf.keras.regularizers.l2(l=1e-5),
                  #  activity_regularizer=tf.keras.regularizers.l2(l=1e-5),
                   kernel_initializer='random_uniform'),
            input_shape=mmw_razi_input))
    mmw_razi_TDCNN.add(TimeDistributed(tf.keras.layers.LeakyReLU(alpha=0.1)))
    mmw_razi_TDCNN.add(TimeDistributed(BatchNormalization()))
    mmw_razi_TDCNN.add(TimeDistributed(
        Conv2D(filters=16, kernel_size=(3, 3), data_format=channel_mode,
              #  kernel_regularizer=tf.keras.regularizers.l2(l=1e-5),
              #  bias_regularizer=tf.keras.regularizers.l2(l=1e-5),
              #  activity_regularizer=tf.keras.regularizers.l2(l=1e-5)
               )))
    mmw_razi_TDCNN.add(TimeDistributed(tf.keras.layers.LeakyReLU(alpha=0.1)))
    mmw_razi_TDCNN.add(TimeDistributed(BatchNormalization()))
    mmw_razi_TDCNN.add(TimeDistributed(MaxPooling2D(pool_size=2)))
    # mmw_razi_TDCNN.add(TimeDistributed(
    #     Conv2D(filters=32, kernel_size=(3, 3), data_format=channel_mode,
    #            kernel_regularizer=tf.keras.regularizers.l2(l=0.01),
    #            bias_regularizer=tf.keras.regularizers.l2(l=0.01))))
    # mmw_rdpl_TDCNN.add(TimeDistributed(tf.keras.layers.LeakyReLU(alpha=0.1)))
    # mmw_razi_TDCNN.add(TimeDistributed(BatchNormalization()))
    # mmw_razi_TDCNN.add(TimeDistributed(MaxPooling2D(pool_size=2)))
    mmw_razi_TDCNN.add(TimeDistributed(Flatten()))  # this should be where layers meets

    merged = concatenate([mmw_rdpl_TDCNN.output, mmw_razi_TDCNN.output])  # concatenate two feature extractors
    regressive_tensor = LSTM(units=32, return_sequences=True, kernel_initializer='random_uniform',
                            #  kernel_regularizer=tf.keras.regularizers.l2(l=1e-4),
                            #  recurrent_regularizer=tf.keras.regularizers.l2(l=1e-5),
                            #  activity_regularizer=tf.keras.regularizers.l2(l=1e-5)
                             )(merged)
    regressive_tensor = Dropout(rate=0.5)(regressive_tensor)
    regressive_tensor = LSTM(units=32, return_sequences=False, kernel_initializer='random_uniform',
                            #  kernel_regularizer=tf.keras.regularizers.l2(l=1e-4),
                            #  recurrent_regularizer=tf.keras.regularizers.l2(l=1e-5),
                            #  activity_regularizer=tf.keras.regularizers.l2(l=1e-5)
                             )(regressive_tensor)
    regressive_tensor = Dropout(rate=0.5)(regressive_tensor)

    regressive_tensor = Dense(units=256,
                              # kernel_regularizer=tf.keras.regularizers.l2(l=1e-4),
                              # bias_regularizer=tf.keras.regularizers.l2(l=1e-5),
                              # activity_regularizer=tf.keras.regularizers.l2(l=1e-5)
                              )(regressive_tensor)
    regressive_tensor = Dropout(rate=0.5)(regressive_tensor)
    regressive_tensor = Dense(len(classes), activation='softmax', kernel_initializer='random_uniform')(regressive_tensor)

    model = Model(inputs=[mmw_rdpl_TDCNN.input, mmw_razi_TDCNN.input], outputs=regressive_tensor)
    adam = tf.keras.optimizers.Adam(lr=5e-5, decay=1e-7)
    model.compile(optimizer=adam, loss='categorical_crossentropy', metrics=['accuracy'])
    return model



def make_model_simple(classes, points_per_sample, channel_mode='channels_last'):
    # creates the Time Distributed CNN for range Doppler heatmap ##########################
    mmw_rdpl_input = (int(points_per_sample),) + rd_shape + (1,) if channel_mode == 'channels_last' else (points_per_sample, 1) + rd_shape
    # range doppler shape here
    mmw_rdpl_TDCNN = Sequential()
    mmw_rdpl_TDCNN.add(
        TimeDistributed(
            Conv2D(filters=4, kernel_size=(3, 3), data_format=channel_mode,
                   kernel_regularizer=tf.keras.regularizers.l2(0.0005),
                   kernel_initializer='random_uniform'),
            input_shape=mmw_rdpl_input))
    # mmw_rdpl_TDCNN.add(TimeDistributed(LeakyReLU(alpha=0.1)))
    mmw_rdpl_TDCNN.add(TimeDistributed(BatchNormalization()))
    mmw_rdpl_TDCNN.add(TimeDistributed(Flatten()))  # this should be where layers meets

    # creates the Time Distributed CNN for range Azimuth heatmap ###########################
    mmw_razi_input = (int(points_per_sample),) + ra_shape + (1,)  if channel_mode == 'channels_last' else (points_per_sample, 1) + ra_shape
    mmw_razi_TDCNN = Sequential()
    mmw_razi_TDCNN.add(
        TimeDistributed(
            Conv2D(filters=4, kernel_size=(3, 3), data_format=channel_mode,
                   kernel_regularizer=tf.keras.regularizers.l2(0.0005),
                   kernel_initializer='random_uniform'),
            input_shape=mmw_razi_input))
    # mmw_rdpl_TDCNN.add(TimeDistributed(LeakyReLU(alpha=0.1)))
    mmw_razi_TDCNN.add(TimeDistributed(BatchNormalization()))
    mmw_razi_TDCNN.add(TimeDistributed(Flatten()))  # this should be where layers meets

    merged = concatenate([mmw_rdpl_TDCNN.output, mmw_razi_TDCNN.output])  # concatenate two feature extractors
    regressive_tensor = LSTM(units=32, return_sequences=False, kernel_initializer='random_uniform')(merged)
    regressive_tensor = Dropout(rate=0.2)(regressive_tensor)

    regressive_tensor = Dense(units=32)(regressive_tensor)
    regressive_tensor = Dropout(rate=0.2)(regressive_tensor)
    regressive_tensor = Dense(len(classes), activation='softmax', kernel_initializer='random_uniform')(regressive_tensor)

    model = Model(inputs=[mmw_rdpl_TDCNN.input, mmw_razi_TDCNN.input], outputs=regressive_tensor)
    adam = tf.keras.optimizers.Adam(lr=1e-5, decay=1e-7)
    model.compile(optimizer=adam, loss='categorical_crossentropy', metrics=['accuracy'])
    return model



def idp_preprocess(data, char_set, input_interval, sensor_sample_points_dict, sensor_features_dict: dict, labeled_sample_dict: dict):
    """
    In this implementation, the tail of the data that does not make up an input_interval is automatically ignored.
    However, it is important to note that if the actual last sample does not expand a full input_interval, it also will
    be discarded. To make up for this, during data collection, always end recording with a brief PAUSE after the last
    gesture; at the same time, ensure the pause is not longer than an input_interval (this is not as hard as it sounds).
    :param labeled_sample_dict:
    :param sensor_features_dict: {'mmw': ('range_doppler', 'range_azi')}
    :param path:
    :param input_interval:
    :param classes:
    :param num_repeat:
    :param period: frame period in milliseconds
    """
    sensor_num_frame = dict([(sensor_name, (sample_per_points * len(char_set))) for sensor_name, sample_per_points in sensor_sample_points_dict.items()])
    # TODO change feature_data to a generator
    sensor_ts_data = [(sensor, data[sensor]['timestamps'], [(f, data[sensor][f]) for f in feature_list])
                      for sensor, feature_list in sensor_features_dict.items()]
    list_feature_samples = []
    for sensor, list_ts, list_sensor_data in sensor_ts_data:
        if len(list_sensor_data[0][1]) == sensor_num_frame[sensor] - 1:
            list_sensor_data = [(feature_name,
                                 frames + [frames[-1]])
                                for feature_name, frames in list_sensor_data]
        elif len(list_ts) > sensor_num_frame[sensor] + 30 or len(list_ts) < sensor_num_frame[sensor] - 1:
            raise Exception()
        list_feature_samples = list_feature_samples + [(sd[0], slice_per(sd[1], step=int(sensor_sample_points_dict[sensor]))) for sd in
                                                       list_sensor_data]  # discard the tail
        list_feature_samples = [(featuren_name, samples[:len(char_set)]) for featuren_name, samples in
                                list_feature_samples]
        # change to NHWC format to comply with GPU requirements
        list_feature_samples = [(featuren_name,
                                 [[np.rollaxis(frm, 0, 3) for frm in smp]
                                  for smp in samples])
                                for featuren_name, samples in list_feature_samples]

        # test for interval time
        try:
            all(len(samples) == len(char_set) for feature_name, samples in list_feature_samples)
        except AssertionError:
            print(
                'len(interval_sensor_data) = ' + str([len(samples) for feature_name, samples in list_feature_samples]))
            print('len(char_set) = ' + str(len(char_set)))
            raise Exception('number of samples does not match the number of characters in the given set')
        interval_ts = slice_per(list_ts, step=int(sensor_sample_points_dict[sensor]))
        interval_ts = interval_ts[:len(char_set)]
        interval_durations = [(max(its) - min(its)) for its in interval_ts]
        interval_variance = np.array(interval_durations) - input_interval
        try:
            assert np.std(interval_variance) < 0.3  # disregard the last
        except AssertionError:
            print('np.std(interval_variance) = ' + str(np.std(interval_variance)))
            raise Exception('Interval std is too high.')
        # try:
        #     assert interval_durations[-1] < 1.0
        # except AssertionError:
        #     print('interval_durations[-1] = ' + str(interval_durations[-1]))
        #     raise Exception('The tail interval is longer than one second.')
    for i, char in enumerate(char_set):
        for ft_name, samples in list_feature_samples:
            try:
                labeled_sample_dict[char][ft_name].append(samples[i])
            except KeyError:
                pass
    return labeled_sample_dict



def load_idp(data_directory, sensor_feature_dict, complete_class, encoder, sensor_sample_points_dict,
             input_interval=4.0):
    '''
    load everything in the given path
    :return:
    '''
    Y = []
    X_dict = dict()
    data_suffix = '_data.mgesf'
    label_suffix = '_label.mgesf'
    feature_names = flatten(list(sensor_feature_dict.values()))
    labeled_sample_dict = dict([(char, dict([(ftn, []) for ftn in feature_names])) for char in complete_class])
    for fn in os.listdir(data_directory):
        if fn.endswith(data_suffix):
            data_path = os.path.join(data_directory, fn)
            label_path = os.path.join(data_directory, fn.replace(data_suffix, '') + label_suffix)
            subject_name = fn.split('_')[-2]
            data = pickle.load(open(data_path, 'rb'))
            label = pickle.load(open(label_path, 'rb'))
            labeled_sample_dict = idp_preprocess(data, char_set=label, input_interval=input_interval,
                                                 sensor_sample_points_dict=sensor_sample_points_dict,
                                                 sensor_features_dict=sensor_feature_dict,
                                                 labeled_sample_dict=labeled_sample_dict)
    # add to x and y
    for char, feature_samples in labeled_sample_dict.items():
        if len(flatten(feature_samples.values())) > 0:
            for ft_name, ft_samples in feature_samples.items():
                if ft_name in X_dict:
                    X_dict[ft_name] = np.concatenate([X_dict[ft_name], np.array(ft_samples)])
                else:
                    X_dict[ft_name] = np.array(ft_samples)
            Y += [char] * len(ft_samples)
    return X_dict, encoder.transform(np.reshape(Y, (-1, 1))).toarray()






def flatten(l):
    return [item for sublist in l for item in sublist]




def transpose(l):
    return list(map(list, zip(*l)))


def slice_per(l, step):
    return [l[index: index + step] for index in range(0, len(l), step)]







def resolve_points_per_sample(period, input_interval):
    return round((1 / period) * input_interval * 1000)





# Allow memory growth for the GPU
# physical_devices = tf.config.experimental.list_physical_devices('GPU')
# tf.config.experimental.set_memory_growth(physical_devices[0], True)
# tf.config.experimental.set_memory_growth(physical_devices[1], True)

idp_complete_classes = [
                        'A', 'B', 'C', 'D', 'E',
                        'F', 'G', 'H', 'I', 'J',
                        'K', 'L', 'M', 'N', 'O',  # accuracy regression
                        'P', 'Q', 'R', 'S', 'T',
                        'U', 'V', 'W', 'X', 'Y',  # accuracy regression
                        'Z', 'Spc', 'Bspc', 'Ent', 'Act'
]

# idp_complete_classes = ['A', 'B', 'C', 'D', 'E']

sensor_feature_dict = {'mmw': ('range_doppler', 'range_azi')}
sensor_period_dict = {'mmw': 33.45}  # period in milliseconds
input_interval = 4.0
sensor_sample_points_dict = dict([(key, (resolve_points_per_sample(value, input_interval))) for key, value in sensor_period_dict.items()])

encoder = OneHotEncoder(categories='auto')
encoder.fit(np.reshape(idp_complete_classes, (-1, 1)))
X_dict, Y = load_idp('/content/drive/My Drive/models/mmWave HCI/data/mGesf/090120_hw_cr0.8',
                     sensor_feature_dict=sensor_feature_dict,
                     complete_class=idp_complete_classes, encoder=encoder, sensor_sample_points_dict=sensor_sample_points_dict)

####################################################################################
X_mmw_rD = X_dict['range_doppler']
X_mmw_rA = X_dict['range_azi']

# min-max normalize
X_mmw_rD = (X_mmw_rD - np.min(X_mmw_rD)) / (np.max(X_mmw_rD) - np.min(X_mmw_rD))
X_mmw_rA = (X_mmw_rA - np.min(X_mmw_rA)) / (np.max(X_mmw_rA) - np.min(X_mmw_rA))

# z normalize
# X_mmw_rD_zScore = (X_mmw_rD - np.mean(X_mmw_rD))/np.std(X_mmw_rD)
# X_mmw_rA_zScore = (X_mmw_rA - np.mean(X_mmw_rA))/np.std(X_mmw_rA)

X_mmw_rD_train, X_mmw_rD_test, Y_train, Y_test = train_test_split(X_mmw_rD, Y, test_size=0.20, random_state=3,
                                                                  shuffle=True)
X_mmw_rA_train, X_mmw_rA_test, Y_train, Y_test = train_test_split(X_mmw_rA, Y, test_size=0.20, random_state=3,
                                                                  shuffle=True)

#####################################################################################
from pathlib import Path
import os
from tensorflow.python.keras.models import load_model
model_dir = '/content/drive/My Drive/models/'
train_completed = False
load_existing = False

sorted_model_paths = sorted(Path(model_dir).iterdir(), key=os.path.getmtime)
if load_existing:
    model_path = sorted_model_paths[-1]  # load the latest model
    print('load model at ' + str(model_path))
    model = load_model(str(model_path))
else:
    model = make_model_no_reg(classes=idp_complete_classes, points_per_sample=sensor_sample_points_dict['mmw'])


es = EarlyStopping(monitor='val_loss', mode='min', verbose=1, patience=500)
mc = ModelCheckpoint(
    '/content/drive/My Drive/models/test/' + 'without_reg' + str(datetime.datetime.now()).replace(':', '-').replace(' ',
                                                                          '_') + '.h5',
    monitor='val_accuracy', mode='max', verbose=1, save_best_only=True)

history = model.fit(([X_mmw_rD_train, X_mmw_rA_train]), Y_train,
                    validation_data=([X_mmw_rD_test, X_mmw_rA_test], Y_test),
                    epochs=50000,
                    batch_size=32, callbacks=[es, mc], verbose=1, )

'''

'''
plt.plot(history.history['accuracy'])
plt.plot(history.history['val_accuracy'])
plt.title('model accuracy')
plt.ylabel('accuracy')
plt.xlabel('epoch')
plt.legend(['train', 'test'], loc='upper left')
plt.show()

# summarize history for loss
plt.plot(history.history['loss'])
plt.plot(history.history['val_loss'])
plt.title('model loss')
plt.ylabel('loss')
plt.xlabel('epoch')
plt.legend(['train', 'test'], loc='upper left')
plt.show()
