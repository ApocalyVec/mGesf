# temporal probability

from keras import Sequential, Model

import datetime
import pickle

from keras import Sequential, optimizers
from keras.callbacks import EarlyStopping, ModelCheckpoint
from keras.layers import Conv3D, MaxPooling2D, Flatten, TimeDistributed, LSTM, Dropout, Dense, BatchNormalization, \
    LeakyReLU, Conv2D, Reshape, concatenate

from keras.regularizers import l2
from keras.engine.saving import load_model

import numpy as np
import os
import matplotlib

import matplotlib.pyplot as plt

from sklearn.model_selection import train_test_split
from sklearn.preprocessing import OneHotEncoder
from Learn.data_in import idp_preprocess, resolve_points_per_sample
from config import rd_shape, ra_shape
from utils.data_utils import plot_confusion_matrix, prepare_x, StreamingMovingAverage, moving_average

########################################################################################################################
# idp_data_dir = ['../data/idp-ABCDE-rpt10', '../data/idp-ABCDE-rpt2']
# num_repeats = [10, 2]
# classes = ['A', 'B', 'C', 'D', 'E']
########################################################################################################################

# idp_data_dir = ['../data/idp-FGHIJ-rpt10']
# num_repeats = [10]
# classes = ['F', 'G', 'H', 'I', 'L']
########################################################################################################################
# idp_data_dir = ['../data/idp-KLMNO-rpt10']
# num_repeats = [10]
# sample_classes = [['K', 'L', 'M', 'N', 'O']]
# classes = ['K', 'L', 'M', 'N', 'O']
########################################################################################################################
# idp_data_dir = ['../data/idp-PQRST-rpt10']
# num_repeats = [10]
# sample_classes = [['P', 'Q', 'R', 'S', 'T']]
# classes = ['P', 'Q', 'R', 'S', 'T']
########################################################################################################################
# idp_data_dir = ['../data/idp-UVWXY-rpt10']
# num_repeats = [10]
# sample_classes = [['U', 'V', 'W', 'X', 'Y']]
# classes = ['U', 'V', 'W', 'X', 'Y']
########################################################################################################################
# idp_data_dir = ['../data/idp-ZSpcBspcEnt-rpt10']
# num_repeats = [10]
# sample_classes = [['Z', 'Spc', 'Bspc', 'Ent']]
# classes = ['Z', 'Spc', 'Bspc', 'Ent']
########################################################################################################################

# idp_data_dir = ['/Users/Leo/Documents/data/idp_29/data/idp-ABCDE-rpt10',
#                 '/Users/Leo/Documents/data/idp_29/data/idp-ABCDE-rpt2',
#                 '/Users/Leo/Documents/data/idp_29/data/idp-FGHIJ-rpt10',
#                 '/Users/Leo/Documents/data/idp_29/data/idp-KLMNO-rpt10',
#                 '/Users/Leo/Documents/data/idp_29/data/idp-PQRST-rpt10',
#                 '/Users/Leo/Documents/data/idp_29/data/idp-UVWXY-rpt10',
#                 '/Users/Leo/Documents/data/idp_29/data/idp-ZSpcBspcEnt-rpt10']
idp_data_dir = ['D:\PycharmProjects\mGesf\data/idp-ABCDE-rpt10',
                'D:\PycharmProjects\mGesf\data/idp-ABCDE-rpt2',
                'D:\PycharmProjects\mGesf\data/idp-FGHIJ-rpt10',
                'D:\PycharmProjects\mGesf\data/idp-KLMNO-rpt10',
                'D:\PycharmProjects\mGesf\data/idp-PQRST-rpt10',
                'D:\PycharmProjects\mGesf\data/idp-UVWXY-rpt10',
                'D:\PycharmProjects\mGesf\data/idp-ZSpcBspcEnt-rpt10']
num_repeats = [10, 2, 10, 10, 10, 10, 10]
sample_classes = [['A', 'B', 'C', 'D', 'E'],
                  ['A', 'B', 'C', 'D', 'E'],  # some of the ABCDE data are repeated twice
                  ['F', 'G', 'H', 'I', 'J'],
                  ['K', 'L', 'M', 'N', 'O'],
                  ['P', 'Q', 'R', 'S', 'T'],
                  ['U', 'V', 'W', 'X', 'Y'],
                  ['Z', 'Spc', 'Bspc', 'Ent']]
classes = ['A', 'B', 'C', 'D', 'E',
           'F', 'G', 'H', 'I', 'J',
           'K', 'L', 'M', 'N', 'O',
           'P', 'Q', 'R', 'S', 'T',
           'U', 'V', 'W', 'X', 'Y',
           'Z', 'Spc', 'Bspc', 'Ent']

assert len(idp_data_dir) == len(num_repeats) == len(sample_classes)  # check the consistency of zip variables
assert set(classes) == set([item for sublist in sample_classes for item in sublist])  # check categorical consistency
########################################################################################################################

interval_duration = 4.0  # how long does one writing take
period = 33  # ms

# classes = set([item for sublist in sample_classes for item in sublist])  # reduce to categorical classes
ls_dicts = \
    [idp_preprocess(dr, interval_duration, classes=cs, num_repeat=nr, period=period)
     for dr, nr, cs in zip(idp_data_dir, num_repeats, sample_classes)]
points_per_sample = int(resolve_points_per_sample(period, interval_duration))

# create input features
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
Y = np.asarray(Y)

encoder = OneHotEncoder(categories='auto')
Y = encoder.fit_transform(np.expand_dims(Y, axis=1)).toarray()

model_name = 'idp_29_2020-05-04_03-24-10.425555'
idp_model = load_model('../models/idp/' + model_name + '.h5')

# make a contiuous temporal sequence A, B, C, D, E
# TODO have this followed by a void character
key_indices = [0, 160, 320, 480, 640]  # A, B, C, D, E
rA_seq = np.array([X_mmw_rA[i] for i in key_indices])
rA_seq = np.reshape(rA_seq,  newshape=[-1] + list(rA_seq.shape[2:]))  # flatten the sample dimension to create temporal sequence
rD_seq = np.array([X_mmw_rD[i] for i in key_indices])
rD_seq = np.reshape(rD_seq,  newshape=[-1] + list(rD_seq.shape[2:]))  # flatten the sample dimension to create temporal sequence

# sample from the temporal sequence
rA_samples = prepare_x(rA_seq, window_size=121, stride=1)
rD_samples = prepare_x(rD_seq, window_size=121, stride=1)

y_pred = idp_model.predict([rD_samples, rA_samples], batch_size=32)

matplotlib.rcParams.update({'font.size': 14})
plt.figure(figsize=(20,6))
for i, col in enumerate(np.transpose(y_pred)[:5]):
    plt.plot(moving_average(col, n=16), label='Predicted gesture: ' + classes[i], linewidth=3)

for i, col in enumerate(np.transpose(y_pred)[5:]):
    plt.plot(moving_average(col, n=16), c='gray', label='Other than A, B, C, D, E') if i == 0 else plt.plot(col, c='gray')

for i in range(1, 5):
    plt.axvline(x=121 * i - 121/2, c='0.3', linewidth=5)

plt.legend(loc=4)
plt.xlabel('Frames')
plt.ylabel('Probability of class prediction')
plt.title('Temporal Probability cross a Continuous Seuqnce of "A, B, C, D, E"')
plt.show()