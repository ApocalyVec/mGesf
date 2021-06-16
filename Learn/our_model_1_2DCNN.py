import datetime

from sklearn.model_selection import train_test_split
from sklearn.preprocessing import OneHotEncoder
import numpy as np
import tensorflow as tf
import seaborn as sn
from livelossplot import PlotLossesKeras
from tensorflow.keras import backend as K
from tensorflow.python.keras.models import load_model
from IPython.display import clear_output
from Learn.data_in import resolve_points_per_sample
from utils.data_utils import load_idp
from tensorflow.python.keras import Sequential, Model, Input
from tensorflow.python.keras.layers import TimeDistributed, Conv2D, BatchNormalization, MaxPooling2D, Flatten, \
    concatenate, LSTM, Dropout, Dense
from utils.learn_utils import make_model, make_model_simple
from tensorflow.python.keras.callbacks import EarlyStopping, ModelCheckpoint
import pickle
import matplotlib.pyplot as plt
from tensorflow.python.client import device_lib
from sklearn.metrics import accuracy_score, classification_report, confusion_matrix, roc_curve, auc
import pandas as pd
import pickle

print(device_lib.list_local_devices())

idp_complete_classes = [
    'A', 'B', 'C', 'D', 'E',
    'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O',  # accuracy regression
    'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y',  # accuracy regression
    'Z', 'Spc', 'Bspc', 'Ent', 'Act'
]

# idp_complete_classes = ['A', 'B', 'C', 'D', 'E']

sensor_feature_dict = {'mmw': ('range_doppler', 'range_azi')}
sensor_period_dict = {'mmw': 33.45}  # period in milliseconds
input_interval = 4.0
sensor_sample_points_dict = dict(
    [(key, (resolve_points_per_sample(value, input_interval))) for key, value in sensor_period_dict.items()])

encoder = OneHotEncoder(categories='auto')
encoder.fit(np.reshape(idp_complete_classes, (-1, 1)))
# 090120_hw
X_dict, Y = load_idp('C:/Users/HaowenWeiJohn/Desktop/2021Spring/DeepLearning/Final_project/HW_data',
                     sensor_feature_dict=sensor_feature_dict,
                     complete_class=idp_complete_classes, encoder=encoder,
                     sensor_sample_points_dict=sensor_sample_points_dict)
save_data_dir = 'C:/Users/HaowenWeiJohn/Desktop/IndexPen_Data/2020_Data/X_dict_Y_Encoder' \
                '/hw_small_without_rearange_without_encoder '

with open(save_data_dir, 'wb') as f:
    pickle.dump([X_dict, Y], f)
#####################################################################################
X_mmw_rD = X_dict['range_doppler']
X_mmw_rA = X_dict['range_azi']

# min-max normalize
X_mmw_rD = (X_mmw_rD - np.min(X_mmw_rD)) / (np.max(X_mmw_rD) - np.min(X_mmw_rD))
X_mmw_rA = (X_mmw_rA - np.min(X_mmw_rA)) / (np.max(X_mmw_rA) - np.min(X_mmw_rA))

# z normalize
# X_mmw_rD_zScore = (X_mmw_rD - np.mean(X_mmw_rD))/np.std(X_mmw_rD)
# X_mmw_rA_zScore = (X_mmw_rA - np.mean(X_mmw_rA))/np.std(X_mmw_rA)

X_mmw_rD_train, X_mmw_rD_test, Y_train, Y_test = train_test_split(X_mmw_rD, Y, test_size=0.30, random_state=2,
                                                                   shuffle=True)
X_mmw_rA_train, X_mmw_rA_test, Y_train, Y_test1 = train_test_split(X_mmw_rA, Y, test_size=0.30, random_state=2,
                                                                   shuffle=True)

X_mmw_rD_valid, X_mmw_rD_test, Y_valid, Y_test = train_test_split(X_mmw_rD_test, Y_test1, test_size=0.5, random_state=2,
                                                                  shuffle=True)
X_mmw_rA_valid, X_mmw_rA_test, Y_valid, Y_test = train_test_split(X_mmw_rA_test, Y_test1, test_size=0.5, random_state=2,
                                                                  shuffle=True)

print(len(X_mmw_rD_train))

# input_RD = Input(shape=(120, 8, 16, 1))
# input_RA = Input(shape=(120, 8, 64, 1))

encoder1 = Sequential()
encoder1.add(tf.keras.layers.InputLayer(input_shape=(120, 8, 16, 1)))
encoder1.add(TimeDistributed(Conv2D(filters=8, kernel_size=(2, 3),
                                    data_format='channels_last')))
encoder1.add(TimeDistributed(tf.keras.layers.LeakyReLU(alpha=0.1)))

encoder1.add(TimeDistributed(MaxPooling2D(pool_size=2)))
encoder1.add(TimeDistributed(BatchNormalization()))
encoder1.add(TimeDistributed(Flatten()))  # or Flatten()
encoder1.add(TimeDistributed(Dense(32, activation='relu')))
# encoder1.add(TimeDistributed(Dropout(rate=0.2)))

# ENcoder2
encoder2 = Sequential()
encoder2.add(tf.keras.layers.InputLayer(input_shape=(120, 8, 64, 1)))
encoder2.add(TimeDistributed(Conv2D(filters=16, kernel_size=(3, 3),
                                    data_format='channels_last')))
encoder2.add(TimeDistributed(tf.keras.layers.LeakyReLU(alpha=0.1)))
encoder2.add(TimeDistributed(MaxPooling2D(pool_size=2)))
encoder2.add(TimeDistributed(BatchNormalization()))
encoder2.add(TimeDistributed(Flatten()))  # or Flatten()
encoder2.add(TimeDistributed(Dense(64, activation='relu')))
# encoder2.add(TimeDistributed(Dropout(rate=0.2)))

merged = concatenate([encoder1.output, encoder2.output])
# merged_out = LSTM(32, return_sequences=True, kernel_initializer='random_uniform',
#                   kernel_regularizer=tf.keras.regularizers.l2(l=1e-4),
#                   recurrent_regularizer=tf.keras.regularizers.l2(l=1e-5),
#                   activity_regularizer=tf.keras.regularizers.l2(l=1e-5)
#                   )(merged)
# merged_out = Dropout(rate=0.2)(merged_out)
merged_out = LSTM(32,
                  kernel_initializer='random_uniform',
                  return_sequences=False,
                  )(merged)
merged_out = Dropout(rate=0.2)(merged_out)
merged_out = Dense(256,
                   activation='relu'
                   )(merged_out)
merged_out = Dropout(rate=0.2)(merged_out)
merged_out = Dense(30, activation='softmax', kernel_initializer='random_uniform')(merged_out)

model = Model(inputs=[encoder1.input, encoder2.input], outputs=merged_out)

adam = tf.keras.optimizers.Adam(lr=1e-3, decay=1e-7)
model.compile(loss='categorical_crossentropy', optimizer=adam, metrics=['accuracy'])


model.summary()

es = EarlyStopping(monitor='val_loss', mode='min', verbose=1, patience=10)
mc = ModelCheckpoint(
    filepath='AutoSave/' + str(datetime.datetime.now()).replace(':', '-').replace(' ',
                                                                                  '_') + '.h5',
    monitor='val_acc', mode='max', verbose=1, save_best_only=True)

history = model.fit([X_mmw_rD_train, X_mmw_rA_train], Y_train,
                    validation_data=([X_mmw_rD_valid, X_mmw_rA_valid], Y_valid),
                    epochs=1000,
                    batch_size=64, callbacks=[es, mc], verbose=2, shuffle=True)

with open('log_hist' + str(datetime.datetime.now()).replace(':', '-').replace(' ', '_'), 'wb') as f:
    pickle.dump(history.history, f)

Y_pred1 = model.predict([X_mmw_rD_test, X_mmw_rA_test])

Y_pred = np.argmax(Y_pred1, axis=1)
Y_test = np.argmax(Y_test, axis=1)
test_acc = accuracy_score(Y_test, Y_pred)
print(test_acc)

print('Confusion Matrix')
cm = confusion_matrix(Y_test, Y_pred)
print(cm)
print('Classification Report')
target_names = [
    'A', 'Act', 'B', 'Bspc', 'C', 'D', 'E', 'Ent',
    'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O',  # accuracy regression
    'P', 'Q', 'R', 'S', 'Spc', 'T',
    'U', 'V', 'W', 'X', 'Y',  # accuracy regression
    'Z'
]
print(classification_report(Y_test, Y_pred, target_names=target_names))

df_cm = pd.DataFrame(cm, index=[i for i in target_names],
                     columns=[i for i in target_names])
plt.figure(figsize=(10, 7))
sn.heatmap(df_cm, annot=True)
plt.show()


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
