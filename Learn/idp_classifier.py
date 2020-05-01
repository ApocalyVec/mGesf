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

from sklearn.model_selection import train_test_split
from sklearn.preprocessing import OneHotEncoder
from Learn.data_in import idp_preprocess
from mGesf.config import rd_shape, ra_shape

idp_data_dir = '../data/idp'

interval_duration = 4.0  # how long does one writing take
classes = ['A', 'B', 'C', 'D', 'E']
num_repeat = 10

# labeled_sample_dict, label_list, points_per_sample = idp_preprocess(idp_data_dir, interval_duration, classes,
#                                                                     num_repeat)
points_per_sample = 121
'''
This implementation accepts two branches of input: range doppler and range azimuth. Each are put
through feature extractors on their branch respectively.
The flattened output from the two branch meets and are concatenated together then put in LSTM, 
the network is concluded by FC layers. 
'''

# creates the Time Distributed CNN for range Doppler heatmap ##########################
mmw_rdpl_input = (points_per_sample, 1) + rd_shape  # range doppler shape here
mmw_rdpl_TDCNN = Sequential()
mmw_rdpl_TDCNN.add(
    TimeDistributed(
        Conv2D(filters=8, kernel_size=(3, 3), data_format='channels_first',
               # kernel_regularizer=l2(0.0005),
               kernel_initializer='random_uniform'),
        input_shape=mmw_rdpl_input))
# mmw_rdpl_TDCNN.add(TimeDistributed(LeakyReLU(alpha=0.1)))
mmw_rdpl_TDCNN.add(TimeDistributed(BatchNormalization()))
mmw_rdpl_TDCNN.add(TimeDistributed(
    Conv2D(filters=8, kernel_size=(3, 3), data_format='channels_first')))
# mmw_rdpl_TDCNN.add(TimeDistributed(LeakyReLU(alpha=0.1)))
mmw_rdpl_TDCNN.add(TimeDistributed(BatchNormalization()))
mmw_rdpl_TDCNN.add(TimeDistributed(MaxPooling2D(pool_size=2)))
mmw_rdpl_TDCNN.add(TimeDistributed(Flatten()))  # this should be where layers meets

# creates the Time Distributed CNN for range Azimuth heatmap ###########################
mmw_razi_input = (points_per_sample, 1) + ra_shape  # range azimuth shape here
mmw_razi_TDCNN = Sequential()
mmw_razi_TDCNN.add(
    TimeDistributed(
        Conv2D(filters=8, kernel_size=(3, 3), data_format='channels_first',
               # kernel_regularizer=l2(0.0005),
               kernel_initializer='random_uniform'),
        input_shape=mmw_razi_input))
# mmw_rdpl_TDCNN.add(TimeDistributed(LeakyReLU(alpha=0.1)))
mmw_razi_TDCNN.add(TimeDistributed(BatchNormalization()))
mmw_razi_TDCNN.add(TimeDistributed(
    Conv2D(filters=8, kernel_size=(3, 3), data_format='channels_first')))
# mmw_rdpl_TDCNN.add(TimeDistributed(LeakyReLU(alpha=0.1)))
mmw_razi_TDCNN.add(TimeDistributed(BatchNormalization()))
mmw_razi_TDCNN.add(TimeDistributed(MaxPooling2D(pool_size=2)))
mmw_razi_TDCNN.add(TimeDistributed(Flatten()))  # this should be where layers meets

merged = concatenate([mmw_rdpl_TDCNN.output, mmw_razi_TDCNN.output])  # concatenate two feature extractors
regressive_tensor = LSTM(units=32, return_sequences=True, kernel_initializer='random_uniform')(merged)
regressive_tensor = Dropout(rate=0.2)(regressive_tensor)
regressive_tensor = LSTM(units=32, return_sequences=True, kernel_initializer='random_uniform')(regressive_tensor)
regressive_tensor = Dropout(rate=0.2)(regressive_tensor)

regressive_tensor = Dense(units=128)(regressive_tensor)
regressive_tensor = Dropout(rate=0.2)(regressive_tensor)
regressive_tensor = Dense(len(classes), activation='softmax', kernel_initializer='random_uniform')(regressive_tensor)

model = Model(inputs=[mmw_rdpl_TDCNN.input, mmw_razi_TDCNN.input], outputs=regressive_tensor)

# model.add(LSTM(units=32, return_sequences=True, kernel_initializer='random_uniform'))
# model.add(Dropout(rate=0.2))
#
# mmw_rdpl_TDCNN.add(LSTM(units=32, return_sequences=True, kernel_initializer='random_uniform'))
# mmw_rdpl_TDCNN.add(Dropout(rate=0.2))
#
# mmw_rdpl_TDCNN.add(Dense(units=128))
# mmw_rdpl_TDCNN.add(Dropout(rate=0.2))
#
# mmw_rdpl_TDCNN.add(Dense(len(classes), activation='softmax', kernel_initializer='random_uniform'))
#
# adam = optimizers.adam(lr=1e-5, decay=1e-7)
# # sgd = optimizers.SGD(lr=5e-6, momentum=0.9, decay=1e-6, nesterov=True)
#
# mmw_rdpl_TDCNN.compile(optimizer=adam, loss='categorical_crossentropy', metrics=['accuracy'])
# print('Using Pre-trained Model: ' + pre_trained_path)

# classifying_labels = [1, 2, 3, 4, 5]
# num_classes = len(classifying_labels)

# interval_duration = 4
# sample_per_sec = 20

# timesteps = interval_duration * sample_per_sec
#
# if __name__ == '__main__':
#     dataset_path = 'F:/alldataset/idp_dataset'
#     label_dict_path = 'F:/alldataset/idp_label_dict.p'
#     labels = pickle.load(open(label_dict_path, 'rb'))

# ## Generators
# X = []
# Y = []
# # for i, data in enumerate(sorted(os.listdir(dataset_path), key=lambda x: int(x.strip('.npy').split('_')[2]))):
# for i, data in enumerate(os.listdir(dataset_path)):
#     lb = labels[os.path.splitext(data)[0]]
#     if lb in classifying_labels: # this is not an 'O'
#         print('Loading ' + str(i) + ' of ' + str(len(os.listdir(dataset_path))))
#         X.append(np.load(os.path.join(dataset_path, data)))
#         Y.append(labels[os.path.splitext(data)[0]])
# X = np.asarray(X)
# Y = np.asarray(Y)

# encoder = OneHotEncoder(categories='auto')
# Y = encoder.fit_transform(np.expand_dims(Y, axis=1)).toarray()

# X_train, X_test, Y_train, Y_test = train_test_split(X, Y, test_size=0.20, random_state=3, shuffle=True)

# Build the RNN ###############################################
# if not is_use_pre_train:
# mmw_rdpl_TDCNN = Sequential()
# mmw_rdpl_TDCNN.add(
#     TimeDistributed(
#         Conv3D(filters=16, kernel_size=(3, 3, 3), data_format='channels_first', input_shape=(1, 25, 25, 25),
#                kernel_regularizer=l2(0.0005),
#                kernel_initializer='random_uniform'),
#         input_shape=(timesteps, 1, 25, 25, 25)))
# # mmw_rdpl_TDCNN.add(TimeDistributed(LeakyReLU(alpha=0.1)))
# mmw_rdpl_TDCNN.add(TimeDistributed(BatchNormalization()))
#
# mmw_rdpl_TDCNN.add(TimeDistributed(
#     Conv3D(filters=16, kernel_size=(3, 3, 3), data_format='channels_first')))
# # mmw_rdpl_TDCNN.add(TimeDistributed(LeakyReLU(alpha=0.1)))
# mmw_rdpl_TDCNN.add(TimeDistributed(BatchNormalization()))
#
# mmw_rdpl_TDCNN.add(TimeDistributed(MaxPooling3D(pool_size=(2, 2, 2))))
#
# mmw_rdpl_TDCNN.add(TimeDistributed(Flatten()))
#
# mmw_rdpl_TDCNN.add(LSTM(units=64, return_sequences=True, kernel_initializer='random_uniform'))
# mmw_rdpl_TDCNN.add(Dropout(rate=0.2))
#
# mmw_rdpl_TDCNN.add(LSTM(units=64, return_sequences=True, kernel_initializer='random_uniform'))
# mmw_rdpl_TDCNN.add(Dropout(rate=0.2))
#
# mmw_rdpl_TDCNN.add(LSTM(units=64, return_sequences=False, kernel_initializer='random_uniform'))
# mmw_rdpl_TDCNN.add(Dropout(rate=0.2))
#
# mmw_rdpl_TDCNN.add(Dense(units=128))
# mmw_rdpl_TDCNN.add(Dropout(rate=0.2))
#
# mmw_rdpl_TDCNN.add(Dense(num_classes, activation='softmax', kernel_initializer='random_uniform'))
#
# adam = optimizers.adam(lr=1e-5, decay=1e-7)
# # sgd = optimizers.SGD(lr=5e-6, momentum=0.9, decay=1e-6, nesterov=True)
#
# mmw_rdpl_TDCNN.compile(optimizer=adam, loss='categorical_crossentropy', metrics=['accuracy'])
# # print('Using Pre-trained Model: ' + pre_trained_path)
# # mmw_rdpl_TDCNN = load_model(pre_trained_path)
#
# # add early stopping
# es = EarlyStopping(monitor='val_loss', mode='min', verbose=1, patience=1000)
# mc = ModelCheckpoint(
#     'D:/trained_models/bestSoFar_indexPen_CRNN' + str(datetime.datetime.now()).replace(':', '-').replace(' ',
#                                                                                                          '_') + '.h5',
#     monitor='val_acc', mode='max', verbose=1, save_best_only=True)
#
# history = mmw_rdpl_TDCNN.fit(X_train, Y_train, validation_data=(X_test, Y_test), epochs=epochs,
#                          batch_size=8, callbacks=[es, mc], verbose=1, )
#
# import matplotlib.pyplot as plt
#
# plt.plot(history.history['acc'])
# plt.plot(history.history['val_acc'])
# plt.title('model accuracy')
# plt.ylabel('accuracy')
# plt.xlabel('epoch')
# plt.legend(['train', 'test'], loc='upper left')
# plt.show()
#
# # summarize history for loss
# plt.plot(history.history['loss'])
# plt.plot(history.history['val_loss'])
# plt.title('model loss')
# plt.ylabel('loss')
# plt.xlabel('epoch')
# plt.legend(['train', 'test'], loc='upper left')
# plt.show()
