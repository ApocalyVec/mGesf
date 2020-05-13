from keras import Sequential
from keras.layers import Conv3D, MaxPooling3D, Flatten, TimeDistributed, LSTM, Dropout, Dense, BatchNormalization
from sklearn.model_selection import train_test_split
import matplotlib.pyplot as plt

from sklearn.preprocessing import OneHotEncoder, MinMaxScaler
import numpy as np

import os

input_dir_list = [
    # 'F:/indexPen/csv/ya_0',
    # 'F:/indexPen/csv/ya_1',
    # 'F:/indexPen/csv/ya_3',

    # 'F:/indexPen/csv/zl_0',
    # 'F:/indexPen/csv/zl_1',
    # 'F:/indexPen/csv/zl_3',

    'F:/indexPen/csv/zy_0',
    'F:/indexPen/csv/zy_1',
    'F:/indexPen/csv/zy_2',
    'F:/indexPen/csv/zy_3'
]

X = None
Y = None
for input_dir in input_dir_list:
    data = np.load(os.path.join(input_dir, 'intervaled_3D_stacked.npy'))
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

# MinMax normalize X
# mmScalar = MinMaxScaler()
# X = mmScalar.fit_transform(X).toarrays()
Xmin = X.min()
Xmax = Y.max()
X = (X - Xmin)/(Xmax - Xmin)
# Onehot encode Y ############################################
onehotencoder = OneHotEncoder(categories='auto')
Y = onehotencoder.fit_transform(np.expand_dims(Y, axis=1)).toarray()

# Separate train and test
test_ratio = 0.2

X_train, X_test, Y_train, Y_test = train_test_split(X, Y, test_size=0.10, random_state=int(12))

# Build the RNN ###############################################

# input shape = (samples, num_timesteps, x_dim, y_xim, channels)

# Conv structure #####################
# cnn = Sequential()
# cnn.add(Conv3D(filter=16, kernel_size=(3, 3), data_format='channels_last', input_shape=(100, 100, 100, 1),
#                activation='relu'))
# cnn.add(MaxPooling3D(pool_size=(2, 2, 2)))
# cnn.add(Flatten())
######################################

model = Sequential()
model.add(Conv3D(filters=32, kernel_size=(3, 3, 3), data_format='channels_first', input_shape=(1, 50, 50, 50),
                 activation='relu'))
model.add(BatchNormalization())
model.add(MaxPooling3D(pool_size=(2, 2, 2)))
model.add(Flatten())

model.add(Dense(5, activation='softmax'))
model.compile(optimizer='adam', loss='categorical_crossentropy', metrics=['accuracy'])

history = model.fit(X_train, Y_train, validation_data=(X_test, Y_test), shuffle=True, epochs=500,
                    batch_size=60)

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
