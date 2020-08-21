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
from Learn.data_in import idp_preprocess_legacy, resolve_points_per_sample
from config import rd_shape, ra_shape
from utils.data_utils import plot_confusion_matrix

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
idp_data_dir = ['data/idp-ABCDE-rpt10',
                'data/idp-ABCDE-rpt2',
                'data/idp-FGHIJ-rpt10',
                'data/idp-KLMNO-rpt10',
                'data/idp-PQRST-rpt10',
                'data/idp-UVWXY-rpt10',
                'data/idp-ZSpcBspcEnt-rpt10']
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
    [idp_preprocess_legacy(dr, interval_duration, classes=cs, num_repeat=nr, period=period)
     for dr, nr, cs in zip(idp_data_dir, num_repeats, sample_classes)]
points_per_sample = int(resolve_points_per_sample(period, interval_duration))
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
regressive_tensor = LSTM(units=32, return_sequences=False, kernel_initializer='random_uniform')(regressive_tensor)
regressive_tensor = Dropout(rate=0.2)(regressive_tensor)

regressive_tensor = Dense(units=128)(regressive_tensor)
regressive_tensor = Dropout(rate=0.2)(regressive_tensor)
regressive_tensor = Dense(len(classes), activation='softmax', kernel_initializer='random_uniform')(regressive_tensor)

model = Model(inputs=[mmw_rdpl_TDCNN.input, mmw_razi_TDCNN.input], outputs=regressive_tensor)
adam = optimizers.adam(lr=1e-4, decay=1e-7)

model.compile(optimizer=adam, loss='categorical_crossentropy', metrics=['accuracy'])

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

X_mmw_rD_train, X_mmw_rD_test, Y_train, Y_test = train_test_split(X_mmw_rD, Y, test_size=0.20, random_state=3,
                                                                  shuffle=True)
X_mmw_rA_train, X_mmw_rA_test, Y_train, Y_test = train_test_split(X_mmw_rA, Y, test_size=0.20, random_state=3,
                                                                  shuffle=True)

# add early stopping
es = EarlyStopping(monitor='val_loss', mode='min', verbose=1, patience=1000)
mc = ModelCheckpoint(
    '../models/' + str(datetime.datetime.now()).replace(':', '-').replace(' ',
                                                                          '_') + '.h5',
    monitor='val_acc', mode='max', verbose=1, save_best_only=True)


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

# load the best model
model_name = 'idp_29_2020-05-04_03-24-10.425555'
idp_model = load_model('../models/idp/' + model_name + '.h5')
# save the history
# pickle.dump(history, open('../models/idp/' + model_name + '.hist', 'wb'))
# plot the confusion matrix
y_pred = idp_model.predict([X_mmw_rD_test, X_mmw_rA_test], batch_size=32)

plot_confusion_matrix(Y_test.argmax(axis=1), y_pred.argmax(axis=1), classes=np.array(classes),
                      normalize=True, title='IndexPen Confusion Matrix')
plt.show()

# make temporal probability evolution graph
matching_label_prob_dict = {}
temporal_prep_dict = {}

for j, pack in enumerate(zip(X_mmw_rD_test, X_mmw_rA_test, encoder.inverse_transform(Y_test), Y_test)):
    rD_sample, rA_sample, true_label, encoded_label = pack
    temporal_samples_rD = []
    temporal_samples_rA = []
    # create sliced samples
    print('Working on ' + str(j) + 'th sample')
    for i in range(len(rD_sample)):
        # create padding
        rD_padding = X_mmw_rD_test[j-1][:points_per_sample - i]
        rA_padding = X_mmw_rA_test[j-1][:points_per_sample - i]

        rD_sample_padded = np.concatenate([rD_padding, rD_sample[:i]])
        rA_sample_padded = np.concatenate([rA_padding, rA_sample[:i]])
        # rD_sample_padded = np.concatenate([np.zeros((points_per_sample - i, ) + rD_sample.shape[1:]), rD_sample[:i]])
        # rA_sample_padded = np.concatenate([np.zeros((points_per_sample - i, ) + rA_sample.shape[1:]), rA_sample[:i]])
        temporal_samples_rD.append(rD_sample_padded)
        temporal_samples_rA.append(rA_sample_padded)

    temporal_samples_rD = np.array(temporal_samples_rD)
    temporal_samples_rA = np.array(temporal_samples_rA)
    temporal_pred = idp_model.predict([temporal_samples_rD, temporal_samples_rA], batch_size=121)

    # get the correct label column
    l_col = np.argmax(encoded_label)
    matching_label_prob = temporal_pred[:, l_col]

    if true_label[0] not in matching_label_prob_dict.keys():
        matching_label_prob_dict[true_label[0]] = np.expand_dims(matching_label_prob, axis=0)
        temporal_prep_dict[true_label[0]] = np.expand_dims(temporal_pred, axis=0)
    else:
        matching_label_prob_dict[true_label[0]] = np.concatenate([np.expand_dims(matching_label_prob, axis=0), matching_label_prob_dict[true_label[0]]], axis=0)
        temporal_prep_dict[true_label[0]] = np.concatenate([np.expand_dims(temporal_pred, axis=0), temporal_prep_dict[true_label[0]]], axis=0)

# plot the temporal evolution of each class
font = {'family': 'DejaVu Sans',
        'weight': 'bold',
        'size': 14}
matplotlib.rc('font', **font)
for i, c in enumerate(classes):
    matching_label_prob_temporal = np.array([v for k, v in matching_label_prob_dict.items() if k == c])[0]
    matching_label_prob_temporal = np.mean(matching_label_prob_temporal, axis=0)
    plt.plot(np.linspace(0, 4, 121), matching_label_prob_temporal,  label=c)
    plt.xlabel('Time since gesture onset (sec)')
    plt.ylabel('P')
    plt.legend()
    plt.ylim((0.0, 1.0))
    if not i % 5:  # plot the figure for every 5 classes
        plt.show()
