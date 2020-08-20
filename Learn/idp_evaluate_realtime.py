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


idp_model = load_model('D:\PcProjects\mGesf\models\idp\idp_29_2020-05-04_03-24-10.425555.h5')

rA_samples = prepare_x(rA_seq, window_size=121, stride=1)
rD_samples = prepare_x(rD_seq, window_size=121, stride=1)

y_pred = idp_model.predict([rD_samples, rA_samples], batch_size=32)
y_pred = y_pred[60:len(y_pred) - 60]

# plottings
matplotlib.rcParams.update({'font.size': 14})
plt.figure(figsize=(20, 6))
is_plotted_others = False

for i, col in enumerate(np.transpose(y_pred)):
    if i in valid_indices:
        plt.plot(moving_average(col, n=16), label='Predicted gesture: ' + index_class_dict[i], linewidth=3)
    else:
        plt.plot(moving_average(col, n=16), c='gray', label='Gestures for other chars') if not is_plotted_others else plt.plot(col,                                                                                                        c='gray')
        is_plotted_others = True

# plot char separation lines
# for i in range(1, len(key_indices) - 2):
#     plt.axvline(x=121 * i, c='0.3', linewidth=5)

debouncer_frame_threshold = 30
debouncer_prob_threshold = 0.9
debouncer = [0] * len(classes)
for i, frame_pred in enumerate(y_pred):
    break_indices = np.argwhere(frame_pred > debouncer_prob_threshold)
    for bi in break_indices:
        bi = bi[0]
        debouncer[bi] = debouncer[bi] + 1
        if debouncer[bi] > debouncer_frame_threshold:
            plt.plot([i], [0.9], 'bo')
            plt.text(i, 0.95, index_class_dict[bi] + 'Detected ', fontsize=12, c='blue')
            debouncer = [0] * len(classes)

# plt.legend(loc=4)
plt.xlabel('Frames (30 frames per second)')
plt.ylabel('Probability of class prediction')
plt.title('Temporal Probability cross a Continuous Sequence of "A, B, C, D, E"')
plt.title('Temporal Probability cross a Continuous Sequence of "H, E, L, L, O, Space, W, O, R, L, D, Enter", with Debouncer Detection')
plt.show()
