from sklearn.model_selection import train_test_split
from sklearn.preprocessing import OneHotEncoder
from tensorflow.python.keras.models import load_model

from Learn.data_in import resolve_points_per_sample
from utils.data_utils import load_idp_new_and_legacy
from tensorflow.python.keras.callbacks import EarlyStopping, ModelCheckpoint
import datetime
import matplotlib.pyplot as plt

import numpy as np

from utils.learn_utils import make_model
import tensorflow as tf

# Allow memory growth for the GPU
gpu_devices = tf.config.experimental.list_physical_devices('GPU')
for device in gpu_devices: tf.config.experimental.set_memory_growth(device, True)

batch_size = 16

idp_complete_classes = [
    'A', 'B', 'C', 'D', 'E',
    'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O',  # accuracy regression
    'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y',  # accuracy regression
    'Z', 'Spc', 'Bspc', 'Ent', 'Act'
]

sensor_feature_dict = {'mmw': ('range_doppler', 'range_azi')}
sensor_period_dict = {'mmw': 33.45}  # period in milliseconds
input_interval = 4.0
sensor_sample_points_dict = dict(
    [(key, (resolve_points_per_sample(value, input_interval))) for key, value in sensor_period_dict.items()])

encoder = OneHotEncoder(categories='auto')
encoder.fit(np.reshape(idp_complete_classes, (-1, 1)))

X_mmw_rD, X_mmw_rA, Y = load_idp_new_and_legacy('/media/apocalyvec/Seagate Backup Plus Drive/research/mgesf/data/090120_hw',
                                                sensor_feature_dict=sensor_feature_dict,
                                                complete_class=idp_complete_classes, encoder=encoder,
                                                sensor_sample_points_dict=sensor_sample_points_dict)

X_mmw_rD = (X_mmw_rD - np.min(X_mmw_rD)) / (np.max(X_mmw_rD) - np.min(X_mmw_rD))
X_mmw_rA = (X_mmw_rA - np.min(X_mmw_rA)) / (np.max(X_mmw_rA) - np.min(X_mmw_rA))

X_mmw_rD_train, X_mmw_rD_test, Y_train, Y_test = train_test_split(X_mmw_rD, Y, test_size=0.20, random_state=3,
                                                                  shuffle=True)
X_mmw_rA_train, X_mmw_rA_test, Y_train, Y_test = train_test_split(X_mmw_rA, Y, test_size=0.20, random_state=3,
                                                                  shuffle=True)

from pathlib import Path
import os
model_dir = '/media/apocalyvec/Seagate Backup Plus Drive/research/mgesf/results/models/idp_hw_test/'
train_completed = False
load_existing = False

while not train_completed:
    sorted_model_paths = sorted(Path(model_dir).iterdir(), key=os.path.getmtime)
    if load_existing:
        model_path = sorted_model_paths[-1]  # load the latest model
        print('load model at ' + str(model_path))
        model = load_model(str(model_path))
    else:
        model = make_model(classes=idp_complete_classes, points_per_sample=sensor_sample_points_dict['mmw'],
                           channel_mode='channels_first', batch_size=batch_size)

    es = EarlyStopping(monitor='val_loss', mode='min', verbose=1, patience=250)
    mc = ModelCheckpoint(
        model_dir + 'with_reg' + str(datetime.datetime.now()).replace(':', '-').replace(' ',
                                                                              '_') + '.h5',
        monitor='val_accuracy', mode='max', verbose=1, save_best_only=True)

    try:
        history = model.fit(([X_mmw_rD_train, X_mmw_rA_train]), Y_train,
                            validation_data=([X_mmw_rD_test, X_mmw_rA_test], Y_test),
                            epochs=1800,
                            batch_size=batch_size, callbacks=[es, mc], verbose=1, )
        train_completed = True
    except:
        print('exception caught, clearing tf session and reloading model')
        load_existing = True
        tf.keras.backend.clear_session()
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
