import datetime

from keras import Sequential, optimizers
from keras.callbacks import EarlyStopping, ModelCheckpoint
from keras.layers import Conv3D, MaxPooling3D, Flatten, TimeDistributed, LSTM, Dropout, Dense, BatchNormalization
from keras.regularizers import l2, l1
from sklearn.model_selection import train_test_split

from sklearn.preprocessing import OneHotEncoder
import numpy as np

import os
import time

input_dir_list = [
    'F:/indexPen/csv_augmented/zr_0',
    'F:/indexPen/csv_augmented/zr_1',

    'F:/indexPen/csv_augmented/py_0',
    'F:/indexPen/csv_augmented/py_1',

    'F:/indexPen/csv_augmented/ya_0',
    'F:/indexPen/csv_augmented/ya_1',

    'F:/indexPen/csv_augmented/zl_0',
    'F:/indexPen/csv_augmented/zl_1',

    'F:/indexPen/csv_augmented/zy_0',
    'F:/indexPen/csv_augmented/zy_1',
]

X = None
Y = None
for input_dir in input_dir_list:
    data_list = [ np.load(os.path.join(input_dir, 'intervaled_3D_volumes_25x_clipping.npy')),
                  np.load(os.path.join(input_dir, 'intervaled_3D_volumes_25x_trans_clipping.npy'))]
    label_array = np.load(os.path.join(input_dir, 'label_array.npy'))

    for data in data_list:

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
test_ratio = 0.1
X_train, X_test, Y_train, Y_test = train_test_split(X, Y, test_size=0.10, random_state=int(12))

# Build the RNN ###############################################

model = Sequential()
model.add(
    TimeDistributed(Conv3D(filters=16, kernel_size=(3, 3, 3), data_format='channels_first', input_shape=(1, 25, 25, 25),
                           activation='relu', kernel_regularizer=l2(0.0005)), input_shape=(100, 1, 25, 25, 25)))
model.add(TimeDistributed(BatchNormalization()))
model.add(TimeDistributed(MaxPooling3D(pool_size=(2, 2, 2))))

# model.add(
#     TimeDistributed(Conv3D(filters=32, kernel_size=(3, 3, 3), data_format='channels_first',
#                            activation='relu', kernel_regularizer=l2(0.0005))))
# model.add(TimeDistributed(BatchNormalization()))
# model.add(TimeDistributed(MaxPooling3D(pool_size=(2, 2, 2))))

model.add(TimeDistributed(Flatten()))

model.add(LSTM(units=64, return_sequences=True))
model.add(Dropout(rate=0.5))

model.add(LSTM(units=64, return_sequences=False))
model.add(Dropout(rate=0.5))

model.add(Dense(5, activation='softmax'))

epochs = 5000

adam = optimizers.adam(lr=1e-5, decay=1e-2/epochs)
model.compile(optimizer=adam, loss='categorical_crossentropy', metrics=['accuracy'])

# add early stopping
es = EarlyStopping(monitor='val_loss', mode='min', verbose=1, patience=250)
mc = ModelCheckpoint('F:/indexPen/trained_models/bestSoFar_indexPen_CRNN' + str(datetime.datetime.now()).replace(':', '-').replace(' ', '_') + '.h5', monitor='val_acc', mode='max', verbose=1, save_best_only=True)

history = model.fit(X_train, Y_train, validation_data=(X_test, Y_test), shuffle=True, epochs=epochs,
                    batch_size=10, callbacks=[es, mc])

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
