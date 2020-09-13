import datetime

from sklearn.model_selection import train_test_split
from sklearn.preprocessing import OneHotEncoder

from tensorflow.python.keras.callbacks import EarlyStopping, ModelCheckpoint

import tensorflow as tf
from tensorflow.python.keras import Sequential, Model
from tensorflow.python.keras.layers import TimeDistributed, Conv2D, BatchNormalization, MaxPooling2D, Flatten, \
    concatenate, LSTM, Dropout, Dense
from collections import OrderedDict
from tensorflow.python.keras.models import load_model

import pickle

import numpy as np
import matplotlib.pyplot as plt
import os

rd_shape = (8, 16)
ra_shape = (8, 64)


def resolve_points_per_sample(period, input_interval):
    return round((1 / period) * input_interval * 1000)


def flatten(l):
    return [item for sublist in l for item in sublist]


def transpose(l):
    return list(map(list, zip(*l)))


def slice_per(l, step):
    return [l[index: index + step] for index in range(0, len(l), step)]


def resolve_sample_feature(ls_dict, lb, f_dict, s_slice, sensor):
    for feature_name, f_array in f_dict.items():
        if feature_name not in ls_dict[lb][sensor].keys():
            ls_dict[lb][sensor][feature_name] = \
                np.expand_dims(f_array[s_slice], axis=0)  # expand dim: sample
        else:
            ls_dict[lb][sensor][feature_name] = \
                np.concatenate([ls_dict[lb][sensor][feature_name],
                                np.expand_dims(f_array[s_slice], axis=0)])


def idp_preprocess_legacy(path, input_interval, classes, num_repeat, period=33):
    """
    In this implementation, the tail of the data that does not make up an input_interval is automatically ignored.
    However, it is important to note that if the actual last sample does not expand a full input_interval, it also will
    be discarded. To make up for this, during data collection, always end recording with a brief PAUSE after the last
    gesture; at the same time, ensure the pause is not longer than an input_interval (this is not as hard as it sounds).
    :param path:
    :param input_interval:
    :param classes:
    :param num_repeat:
    :param period: frame period in milliseconds
    """
    mgesf_data_list = (pickle.load(open(os.path.join(path, p), 'rb')) for p in os.listdir(path))

    # self.buffer = {'mmw': {'timestamps': [], 'range_doppler': [], 'range_azi': [], 'detected_points': []}}

    labeled_sample_dict = OrderedDict([(label, {'mmw': {}}) for label in classes])
    sample_frame_durations = []

    sample_num_expected = len(classes) * num_repeat  # expected number of samples per data block
    points_per_sample = resolve_points_per_sample(period, input_interval)

    mmw_features = ['range_doppler', 'range_azi']

    for d, p in zip(mgesf_data_list, os.listdir(path)):
        sample_ts_list = []

        mmw_list = d['mmw']
        label_list = [[x for i in range(num_repeat)] for x in classes]
        label_list = [item for sublist in label_list for item in sublist]  # flattten

        ts_array = np.array(mmw_list['timestamps'])

        # checks data shapes
        # invalid_rdpl_list = [(i, a) for i, a in enumerate(mmw_list['range_doppler']) if a.shape != (8, 16)]
        # invalid_rzm_list = [(i, a) for i, a in enumerate(mmw_list['range_azi']) if a.shape != (8, 64)]

        mmwf_dict = dict([(f, np.array(mmw_list[f])) for f in mmw_features])

        # slice the ts list by the interval duration
        ts_array = ts_array - ts_array[0]  # ground by the first frame's timestamp
        ts_index_last = 0
        for sample_index in range(1, sample_num_expected + 1):
            for ts_index, ts in enumerate(ts_array):
                if ts - input_interval * sample_index >= 0.:  # end of a sample
                    if ts_index - ts_index_last != int(points_per_sample):
                        print('Too many samples: ' + str(ts_index - ts_index_last))
                        print('Using Slice: ' + str(
                            slice(ts_index_last, ts_index_last + int(points_per_sample))) + ' instead of ' + str(
                            slice(ts_index_last, ts_index)))
                    sample_slice = slice(ts_index_last, ts_index if ts_index - ts_index_last == int(
                        points_per_sample) else ts_index_last + int(points_per_sample))
                    sample_ts = ts_array[sample_slice]
                    lb = label_list[sample_index - 1]
                    # lb = classes[(samp le_index - 1) % len(classes)]  # sample_index decrement for class index offset
                    sample_frame_durations.append(max(sample_ts) - min(sample_ts))
                    sample_ts_list.append(sample_ts)

                    # append the features and labels
                    resolve_sample_feature(labeled_sample_dict, lb, mmwf_dict, sample_slice, 'mmw')  # mmw features

                    label_list.append(lb)  # labels
                    ts_index_last = ts_index
                    break  # break to the next sample
        if len(sample_ts_list) != sample_num_expected:
            raise Exception(
                'Sample number mismatch, got ' + str(len(sample_ts_list)) + ', expected: ' + str(sample_num_expected) +
                '\n for ' + p)

    return labeled_sample_dict


def idp_preprocess(data, char_set, input_interval, sensor_sample_points_dict, sensor_features_dict: dict,
                   labeled_sample_dict: dict, channel_mode='channels_last'):
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
    sensor_num_frame = dict([(sensor_name, (sample_per_points * len(char_set))) for sensor_name, sample_per_points in
                             sensor_sample_points_dict.items()])
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
        list_feature_samples = list_feature_samples + [
            (sd[0], slice_per(sd[1], step=int(sensor_sample_points_dict[sensor]))) for sd in
            list_sensor_data]  # discard the tail
        list_feature_samples = [(featuren_name, samples[:len(char_set)]) for featuren_name, samples in
                                list_feature_samples]
        # change to NHWC format to comply with GPU requirements
        list_feature_samples = [(featuren_name,
                                 [[np.rollaxis(frm, 0, 3) for frm in smp]
                                  for smp in samples])
                                for featuren_name, samples in
                                list_feature_samples] if channel_mode == 'channels_last' else list_feature_samples

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


def load_idp_new_and_legacy(data_directory, sensor_feature_dict, complete_class, encoder, sensor_sample_points_dict,
                            input_interval=4.0, legacy_root=None):
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
    for i, fn in enumerate(os.listdir(data_directory)):
        print('loading file ' + str(i) + ' of ' + str(len(os.listdir(data_directory))) + ', file name is ' + fn)
        if fn.endswith(data_suffix):
            data_path = os.path.join(data_directory, fn)
            label_path = os.path.join(data_directory, fn.replace(data_suffix, '') + label_suffix)
            subject_name = fn.split('_')[-2]
            data = pickle.load(open(data_path, 'rb'))
            label = pickle.load(open(label_path, 'rb'))
            labeled_sample_dict = idp_preprocess(data, char_set=label, input_interval=input_interval,
                                                 sensor_sample_points_dict=sensor_sample_points_dict,
                                                 sensor_features_dict=sensor_feature_dict,
                                                 labeled_sample_dict=labeled_sample_dict, channel_mode='channels_first')
    # add to x and y
    print('creating x, y samples')
    for char, feature_samples in labeled_sample_dict.items():
        if len(flatten(feature_samples.values())) > 0:
            for ft_name, ft_samples in feature_samples.items():
                if ft_name in X_dict:
                    X_dict[ft_name] = np.concatenate([X_dict[ft_name], np.array(ft_samples)])
                else:
                    X_dict[ft_name] = np.array(ft_samples)
            Y += [char] * len(ft_samples)

    X_mmw_rD = X_dict['range_doppler']
    X_mmw_rA = X_dict['range_azi']

    if legacy_root is not None:
        print('loading legacy zl data')
        X_mmw_rD_legacy, X_mmw_rA_legacy, Y_legacy = idp_legacy_xy(legacy_root)
        X_mmw_rD = np.concatenate((X_mmw_rD, X_mmw_rD_legacy))
        X_mmw_rA = np.concatenate((X_mmw_rA, X_mmw_rA_legacy))
        Y = Y + Y_legacy

    return X_mmw_rD, X_mmw_rA, encoder.transform(np.reshape(Y, (-1, 1))).toarray()


def idp_legacy_xy(legacy_root):
    idp_data_dir = ['idp-ABCDE-rpt10',
                    'idp-ABCDE-rpt2',
                    'idp-FGHIJ-rpt10',
                    'idp-KLMNO-rpt10',
                    'idp-PQRST-rpt10',
                    'idp-UVWXY-rpt10',
                    'idp-ZSpcBspcEnt-rpt10'
                    ]
    idp_data_dir = [os.path.join(legacy_root, x) for x in idp_data_dir]
    num_repeats = [10, 2,
                   10, 10, 10, 10, 10
                   ]
    sample_classes = [['A', 'B', 'C', 'D', 'E'],
                      ['A', 'B', 'C', 'D', 'E'],  # some of the ABCDE data are repeated twice
                      ['F', 'G', 'H', 'I', 'J'],
                      ['K', 'L', 'M', 'N', 'O'],
                      ['P', 'Q', 'R', 'S', 'T'],
                      ['U', 'V', 'W', 'X', 'Y'],
                      ['Z', 'Spc', 'Bspc', 'Ent']
                      ]
    classes = ['A', 'B', 'C', 'D', 'E',
               'F', 'G', 'H', 'I', 'J',
               'K', 'L', 'M', 'N', 'O',
               'P', 'Q', 'R', 'S', 'T',
               'U', 'V', 'W', 'X', 'Y',
               'Z', 'Spc', 'Bspc', 'Ent'
               ]

    assert len(idp_data_dir) == len(num_repeats) == len(sample_classes)  # check the consistency of zip variables
    assert set(classes) == set(
        [item for sublist in sample_classes for item in sublist])  # check categorical consistency

    interval_duration = 4.0  # how long does one writing take
    period = 33.45  # ms

    # classes = set([item for sublist in sample_classes for item in sublist])  # reduce to categorical classes
    ls_dicts = \
        [idp_preprocess_legacy(dr, interval_duration, classes=cs, num_repeat=nr, period=period)
         for dr, nr, cs in zip(idp_data_dir, num_repeats, sample_classes)]

    Y = []
    X_mmw_rD = []
    X_mmw_rA = []

    # add to x and y
    for lsd in ls_dicts:
        for key, value in lsd.items():
            X_mmw_rD += [d for d in value['mmw']['range_doppler']]
            X_mmw_rA += [a for a in value['mmw']['range_azi']]
            Y += [key for i in range(value['mmw']['range_doppler'].shape[0])]
            pass

    X_mmw_rD = np.asarray(X_mmw_rD)
    X_mmw_rA = np.asarray(X_mmw_rA)

    return X_mmw_rD, X_mmw_rA, Y


# load data with leave-one-subject-out
idp_complete_classes = [
    'A', 'B', 'C', 'D', 'E',
    'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O',  # accuracy regression
    'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y',  # accuracy regression
    'Z', 'Spc', 'Bspc', 'Ent', 'Act'
]

sensor_feature_dict = {'mmw': ('range_doppler', 'range_azi')}
sensor_period_dict = {'mmw': 33.45}  # period in milliseconds
input_interval = 4.0
sensor_sample_points_dict = dict(
    [(key, (resolve_points_per_sample(value, input_interval))) for key, value in sensor_period_dict.items()])

encoder = OneHotEncoder(categories='auto')
encoder.fit(np.reshape(idp_complete_classes, (-1, 1)))

X_mmw_rD, X_mmw_rA, Y = load_idp_new_and_legacy('/media/apocalyvec/Seagate Backup Plus Drive/research/mgesf/data/090120_hw',
                                                sensor_feature_dict=sensor_feature_dict,
                                                complete_class=idp_complete_classes, encoder=encoder,
                                                sensor_sample_points_dict=sensor_sample_points_dict,
                                                legacy_root='/media/apocalyvec/Seagate Backup Plus Drive/research/mgesf/data/050120_zl_legacy')

# load a leave-one-subject-out model
model_path = '/media/apocalyvec/Seagate Backup Plus Drive/research/mgesf/results/models/idp_hw_zl_almost_there_acc_098930/2020-09-03_01-51-11.289968.h5'
model = load_model(model_path)

rD_max, rD_min = np.max(X_mmw_rD), np.min(X_mmw_rD)
rA_max, rA_min = np.max(X_mmw_rA), np.min(X_mmw_rA)
# additional processing on the data
X_mmw_rD = (X_mmw_rD - rD_min) / (rD_max - rD_min)
X_mmw_rA = (X_mmw_rA - rA_min) / (rA_max - rA_min)
X_mmw_rD_train, X_mmw_rD_test, Y_train, Y_test = train_test_split(X_mmw_rD, Y, test_size=0.20, random_state=3,
                                                                  shuffle=True)
X_mmw_rA_train, X_mmw_rA_test, Y_train, Y_test = train_test_split(X_mmw_rA, Y, test_size=0.20, random_state=3,
                                                                  shuffle=True)

# sanity check on if the model loaded is correct by evaluating the model on its training data
model.evaluate(x=[X_mmw_rD_test, X_mmw_rA_test], y=Y_test, batch_size=32)
print('removing pretrain train test from memory')
del X_mmw_rD_train, X_mmw_rD_test, X_mmw_rA_train, X_mmw_rA_test, Y_train, Y_test
# load the left-out user's data
X_mmw_rD_leftOut, X_mmw_rA_leftOut, Y_leftOut = load_idp_new_and_legacy(
    '/media/apocalyvec/Seagate Backup Plus Drive/research/mgesf/data/090120_ag',
    sensor_feature_dict=sensor_feature_dict,
    complete_class=idp_complete_classes, encoder=encoder,
    sensor_sample_points_dict=sensor_sample_points_dict)

# min max normalize on the same scale
X_mmw_rD_leftOut = (X_mmw_rD_leftOut - rD_min) / (rD_max - rD_min)
X_mmw_rA_leftOut = (X_mmw_rA_leftOut - rA_min) / (rA_max - rA_min)

# evaluate on the left-out data
model.evaluate(x=[X_mmw_rD_leftOut, X_mmw_rA_leftOut], y=Y_leftOut, batch_size=32)

# transfer learning
# obtain per-class samples from the left-out user
X_mmw_rA_leftOut_per_class_samples = [(cls,
                                       X_mmw_rA_leftOut[
                                           np.all(Y_leftOut == encoder.transform([[cls]]).toarray(), axis=1)]
                                       ) for cls in idp_complete_classes]
X_mmw_rD_leftOut_per_class_samples = [(cls,
                                       X_mmw_rD_leftOut[
                                           np.all(Y_leftOut == encoder.transform([[cls]]).toarray(), axis=1)]
                                       ) for cls in idp_complete_classes]

# create calibration sets
calib_add_factor = 5
num_calib = 20
X_mmw_rD_leftOut_calib = []
X_mmw_rA_leftOut_calib = []
Y_leftOut_calid = []
for i in range(num_calib * calib_add_factor):
    print('Creating calibration samples: ' + str(i) + ' of ' + str(num_calib))
    X_mmw_rD_leftOut_calib.append(np.array([cls_sample[i] for cls, cls_sample in X_mmw_rD_leftOut_per_class_samples]))
    X_mmw_rA_leftOut_calib.append(np.array([cls_sample[i] for cls, cls_sample in X_mmw_rA_leftOut_per_class_samples]))

    y_rD = np.array([encoder.transform([[cls]]).toarray() for cls, cls_sample in X_mmw_rD_leftOut_per_class_samples])
    y_rA = np.array([encoder.transform([[cls]]).toarray() for cls, cls_sample in X_mmw_rD_leftOut_per_class_samples])

    assert np.all(y_rD == y_rA)
    Y_leftOut_calid.append(np.array(y_rD))

# shuffle calib data
from sklearn.utils import shuffle
X_mmw_rD_leftOut_calib, X_mmw_rA_leftOut_calib, Y_leftOut_calid = shuffle(X_mmw_rD_leftOut_calib, X_mmw_rA_leftOut_calib, Y_leftOut_calid, random_state=42)

# perform transfer learning
transfer_loss = []
transfer_accuracy = []

weighted = True
model_dir = '/media/apocalyvec/Seagate Backup Plus Drive/research/mgesf/results/models/idp_cross_user_left_ag/'

for i in range(num_calib):
    print('clearing session staring enw transfer step ---------------------------------------')
    tf.keras.backend.clear_session()
    calib_sample_num = (i + 1) * calib_add_factor
    print('Training on ' + str(calib_sample_num) + ' calibration sets...')
    x_rd = np.reshape(X_mmw_rD_leftOut_calib[:calib_sample_num], (calib_sample_num * 30,) + X_mmw_rD_leftOut_calib[0][0].shape)
    x_ra = np.reshape(X_mmw_rA_leftOut_calib[:calib_sample_num], (calib_sample_num * 30,) + X_mmw_rA_leftOut_calib[0][0].shape)
    y = np.reshape(Y_leftOut_calid[:calib_sample_num], (calib_sample_num * 30,) + Y_leftOut_calid[0][0][0].shape)

    # append old data
    x_rd = np.concatenate([x_rd, X_mmw_rD])
    x_ra = np.concatenate([x_ra, X_mmw_rA])
    y_ = np.concatenate([y, Y])
    print(x_rd.shape)
    print(x_ra.shape)
    print(y.shape)
    # reload model
    model = load_model(model_path)
    # learn for 100 epochs
    es = EarlyStopping(monitor='val_loss', mode='min', verbose=1, patience=250)
    mc = ModelCheckpoint(
        model_dir + '/model.h5',
        monitor='val_accuracy', mode='max', verbose=1, save_best_only=True)
    if not weighted:
        history = model.fit(([x_rd, x_ra]), y_,
                            validation_data=([X_mmw_rD_leftOut, X_mmw_rA_leftOut], Y_leftOut),
                            epochs=1000, shuffle=True, callbacks=[es, mc],
                            batch_size=32)
    else:
        sample_weight = np.ones(shape=(len(y_),))
        sample_weight[:((calib_sample_num) * 30)] = np.array([(len(X_mmw_rD) / ((1) * 30 )) * 1e-2] * calib_sample_num * 30)
        # sample_weight[:((calib_sample_num) * 30)] = np.array([len(X_mmw_rD) / (calib_sample_num) * 30 / 10000.] * calib_sample_num * 30)
        print('Using weighted training, weight is ' + str(sample_weight[0]) + ' for the first ' + str(calib_sample_num * 30) + ' samples')
        history = model.fit(([x_rd, x_ra]), y_,
                            validation_data=([X_mmw_rD_leftOut, X_mmw_rA_leftOut], Y_leftOut),
                            epochs=1200, shuffle=True, sample_weight=sample_weight, callbacks=[es, mc],
                            batch_size=32)
    # load the best performing model
    model = load_model(model_dir + 'model.h5')
    eval_result = model.evaluate(x=[X_mmw_rD_leftOut, X_mmw_rA_leftOut], y=Y_leftOut, batch_size=32)
    transfer_loss.append(eval_result[0])
    transfer_accuracy.append(eval_result[1])
