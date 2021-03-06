import time

import numpy as np
import pandas as pd
import pickle
import os

from sklearn.model_selection import train_test_split

from keras.models import Sequential
from keras.layers import Dense
from keras.layers import LSTM
from keras.layers import Dropout
from keras import optimizers

# get the data
from sklearn.preprocessing import OneHotEncoder

input_dir_list = ['F:\palmpad\csv\ya_0',
                  # 'F:\palmpad\csv\ya_1',  # missing A at index 19,20,21
                  'F:\palmpad\csv\zl_0',
                  # 'F:\palmpad\csv\zl_1',  # wrote duplicate
                  'F:/palmpad/csv/zy_0',
                  'F:/palmpad/csv/zy_1'
                  ]

X = None
Y = None
for input_dir in input_dir_list:
    data = np.load(os.path.join(input_dir, 'intervaled_ts_removed.npy'))
    label_array = np.load(os.path.join(input_dir, 'label_array.npy'))

    # put in the data
    if X is None:
        X = data
    else:
        X = np.concatenate((X, data))

    if Y is None:
        Y = label_array
    else:
        Y = np.concatenate((Y, label_array))

# Onehot encode Y ############################################
onehotencoder = OneHotEncoder(categories='auto')
Y = onehotencoder.fit_transform(np.expand_dims(Y, axis=1)).toarray()

# Separate train and test
test_ratio = 0.2

X_train, X_test, Y_train, Y_test = train_test_split(X, Y, test_size=0.20, random_state=int(time.time()))

# Build the RNN ###############################################
interval_sec = 5
sample_per_sec = 15
sample_per_interval = interval_sec * sample_per_sec
# Initialising the RNN
regressiveClassifier = Sequential()

# batch size = 3337, num_timestep = 100,
regressiveClassifier.add(LSTM(units=400, return_sequences=True, input_shape=(sample_per_interval, 400)))
regressiveClassifier.add(Dropout(rate=0.8))

regressiveClassifier.add(LSTM(units=400, return_sequences=True))
regressiveClassifier.add(Dropout(rate=0.5))

regressiveClassifier.add(LSTM(units=400, return_sequences=True))
regressiveClassifier.add(Dropout(0.5))

regressiveClassifier.add(LSTM(units=400, return_sequences=False))  # or return_sequences is default at False
regressiveClassifier.add(Dropout(rate=0.5))

# dense layer
regressiveClassifier.add(Dense(units=400, kernel_initializer='uniform', activation='relu'))
regressiveClassifier.add(Dropout(rate=0.5))

regressiveClassifier.add(Dense(units=400, kernel_initializer='uniform', activation='relu'))
regressiveClassifier.add(Dropout(rate=0.5))

regressiveClassifier.add(Dense(5, activation='softmax'))

adam_lr5e_4 = optimizers.adam(lr=5e-5, clipnorm=1.)  # use half the learning rate as adam optimizer default
regressiveClassifier.compile(optimizer=adam_lr5e_4, loss='categorical_crossentropy', metrics=['accuracy'])

history = regressiveClassifier.fit(X_train, Y_train, validation_data=(X_test, Y_test), shuffle=True, epochs=100, batch_size=10)

# plot train history
import matplotlib.pyplot as plt

plt.plot(history.history['acc'])
plt.plot(history.history['val_acc'])
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

# save train result
# date = '080819'
# regressiveClassifier.save(os.path.join('F:/palmpad/models', date + 'classifier.h5'))
# pickle.dump(onehotencoder, open(os.path.join('F:/palmpad/models', date + 'encoder.p', 'wb'), 'wb'))

# TODO change learning rate
