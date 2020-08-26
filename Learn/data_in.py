import pickle
import time
import warnings
from collections import OrderedDict

import numpy as np
import os


def resolve_sample_feature(ls_dict, lb, f_dict, s_slice, sensor):
    for feature_name, f_array in f_dict.items():
        if feature_name not in ls_dict[lb][sensor].keys():
            ls_dict[lb][sensor][feature_name] = \
                np.expand_dims(f_array[s_slice], axis=0)  # expand dim: sample
        else:
            ls_dict[lb][sensor][feature_name] = \
                np.concatenate([ls_dict[lb][sensor][feature_name],
                                np.expand_dims(f_array[s_slice], axis=0)])


def resolve_points_per_sample(period, input_interval):
    return (1 / period) * input_interval * 1000


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
                    # lb = classes[(sample_index - 1) % len(classes)]  # sample_index decrement for class index offset
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


def idp_preprocess(data, char_set, input_interval, period, sensor_features_dict: dict, labeled_sample_dict: dict):
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
    sample_frame_durations = []
    points_per_sample = round(resolve_points_per_sample(period, input_interval))
    num_frame = points_per_sample * len(char_set)
    # TODO change feature_data to a generator
    sensor_ts_data = [(data[sensor]['timestamps'], [(f, data[sensor][f]) for f in feature_list])
                      for sensor, feature_list in sensor_features_dict.items()]
    list_feature_samples = []
    for list_ts, list_sensor_data in sensor_ts_data:
        if len(list_sensor_data[0][1]) == num_frame - 1:
            list_sensor_data = [(feature_name,
                                 frames + [frames[-1]])
                                for feature_name, frames in list_sensor_data]
        elif len(list_ts) > num_frame + 1 or len(list_ts) < num_frame - 1:
            raise Exception()
        list_feature_samples = list_feature_samples + [(sd[0], slice_per(sd[1], step=points_per_sample)) for sd in
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
        interval_ts = slice_per(list_ts, step=points_per_sample)
        interval_ts = interval_ts[:len(char_set)]
        interval_durations = [(max(its) - min(its)) for its in interval_ts]
        interval_variance = np.array(interval_durations) - input_interval
        try:
            assert np.std(interval_variance) < 0.1  # disregard the last
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


def flatten(l):
    return [item for sublist in l for item in sublist]


def transpose(l):
    return list(map(list, zip(*l)))


def slice_per(l, step):
    return [l[index: index + step] for index in range(0, len(l), step)]
