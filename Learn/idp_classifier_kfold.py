
import datetime
import pickle

import numpy as np
import os
import matplotlib

import matplotlib.pyplot as plt

from sklearn.model_selection import train_test_split
from sklearn.preprocessing import OneHotEncoder
from tensorflow.python.keras import Sequential, Model
from tensorflow.python.keras.callbacks import EarlyStopping, ModelCheckpoint
from tensorflow.python.keras.layers import TimeDistributed, Conv2D, BatchNormalization, MaxPooling2D, Flatten, \
    concatenate, LSTM, Dropout, Dense
from tensorflow.python.keras.models import load_model
import tensorflow as tf

from Learn.data_in import idp_preprocess_legacy, resolve_points_per_sample
from config import rd_shape, ra_shape
from utils.data_utils import plot_confusion_matrix
from sklearn.model_selection import KFold

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
from utils.learn_utils import make_model

idp_data_dir = ['../data/idp-ABCDE-rpt10',
                '../data/idp-ABCDE-rpt2',
                '../data/idp-FGHIJ-rpt10',
                '../data/idp-KLMNO-rpt10',
                '../data/idp-PQRST-rpt10',
                '../data/idp-UVWXY-rpt10',
                '../data/idp-ZSpcBspcEnt-rpt10']
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

# add early stopping
es = EarlyStopping(monitor='val_loss', mode='min', verbose=1, patience=200)
mc = ModelCheckpoint(
    '../models/' + str(datetime.datetime.now()).replace(':', '-').replace(' ',
                                                                          '_') + '.h5',
    monitor='val_acc', mode='max', verbose=1, save_best_only=True)

# Define per-fold score containers
acc_per_fold = []
loss_per_fold = []
num_folds = 10

kfold = KFold(n_splits=num_folds, shuffle=True)
fold_no = 1
for train, test in kfold.split(X_mmw_rD, Y):
    model = make_model(classes, points_per_sample)
    x_rD_train = X_mmw_rD[train]
    x_rA_train = X_mmw_rA[train]
    y_train = Y[train]

    x_rD_test = X_mmw_rD[test]
    x_rA_test = X_mmw_rA[test]
    y_test = Y[test]

    print('------------------------------------------------------------------------')
    print(f'Training for fold {fold_no} ...')
    history = model.fit(([x_rD_train, x_rA_train]), y_train,
                        validation_data=([x_rD_test, x_rA_test], y_test),
                        epochs=1500,
                        batch_size=32, callbacks=[es, mc], verbose=1, )
    # Generate generalization metrics
    scores = model.evaluate([x_rD_test, x_rA_test], y_test, verbose=0)
    print(
        f'Score for fold {fold_no}: {model.metrics_names[0]} of {scores[0]}; {model.metrics_names[1]} of {scores[1] * 100}%')
    acc_per_fold.append(scores[1] * 100)
    loss_per_fold.append(scores[0])

    # Increase fold number
    fold_no = fold_no + 1
    tf.keras.backend.clear_session()

# == Provide average scores ==
print('------------------------------------------------------------------------')
print('Score per fold')
for i in range(0, len(acc_per_fold)):
  print('------------------------------------------------------------------------')
  print(f'> Fold {i+1} - Loss: {loss_per_fold[i]} - Accuracy: {acc_per_fold[i]}%')
print('------------------------------------------------------------------------')
print('Average scores for all folds:')
print(f'> Accuracy: {np.mean(acc_per_fold)} (+- {np.std(acc_per_fold)})')
print(f'> Loss: {np.mean(loss_per_fold)}')
print('------------------------------------------------------------------------')



