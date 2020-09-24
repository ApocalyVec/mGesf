import os

from sklearn.model_selection import train_test_split
from sklearn.preprocessing import OneHotEncoder
from tensorflow.python.keras.models import load_model
import pickle
from Learn.data_in import resolve_points_per_sample
from utils.data_utils import load_idp_new_and_legacy, prepare_x
from tensorflow.python.keras.callbacks import EarlyStopping, ModelCheckpoint
import datetime
import matplotlib.pyplot as plt
import matplotlib
import numpy as np

from utils.learn_utils import make_model
import tensorflow as tf

# Allow memory growth for the GPU
gpu_devices = tf.config.experimental.list_physical_devices('GPU')
for device in gpu_devices: tf.config.experimental.set_memory_growth(device, True)

idp_complete_classes = [
    'A', 'B', 'C', 'D', 'E',
    'F', 'G', 'H', 'I', 'J',
    'K', 'L', 'M', 'N', 'O',  # accuracy regression
    'P', 'Q', 'R', 'S', 'T',
    'U', 'V', 'W', 'X', 'Y',  # accuracy regression
    'Z', 'Spc', 'Bspc', 'Ent', 'Act'
]

print('loading original data for normalization')
sensor_feature_dict = {'mmw': ('range_doppler', 'range_azi')}
sensor_period_dict = {'mmw': 33.45}  # period in milliseconds
input_interval = 4.0
encoder = OneHotEncoder(categories='auto')
encoder.fit(np.reshape(idp_complete_classes, (-1, 1)))
sensor_sample_points_dict = dict(
    [(key, (resolve_points_per_sample(value, input_interval))) for key, value in sensor_period_dict.items()])

print('loading existing model')
scenario = 'Watch'
tt_split = 0.5
model_dir = 'E:/data/mGesf/091020/0' + scenario + '/model'

model = load_model(os.path.join(model_dir, 'model.h5'))
# model = load_model('D:/PcProjects/mGesf/models/idp_all/2020-09-04_05-41-48.712257.h5')

X_mmw_rD, X_mmw_rA, Y = load_idp_new_and_legacy('E:/data/mGesf/091020/0' + scenario,
                                                sensor_feature_dict=sensor_feature_dict,
                                                complete_class=idp_complete_classes, encoder=encoder,
                                                sensor_sample_points_dict=sensor_sample_points_dict
                                                )

rD_max, rD_min = pickle.load(open('D:/PcProjects/mGesf/data/rD_max.p', 'rb')), pickle.load(
    open('D:/PcProjects/mGesf/data/rD_min.p', 'rb'))
rA_max, rA_min = pickle.load(open('D:/PcProjects/mGesf/data/rA_max.p', 'rb')), pickle.load(
    open('D:/PcProjects/mGesf/data/rA_min.p', 'rb'))

X_mmw_rD = (X_mmw_rD - rD_min) / (rD_max - rD_min)
X_mmw_rA = (X_mmw_rA - rA_min) / (rA_max - rA_min)

X_mmw_rD_train, X_mmw_rD_test, Y_train, Y_test = train_test_split(X_mmw_rD, Y, test_size=tt_split, random_state=3,
                                                                  shuffle=True)
X_mmw_rA_train, X_mmw_rA_test, Y_train, Y_test = train_test_split(X_mmw_rA, Y, test_size=tt_split, random_state=3,
                                                                  shuffle=True)

es = EarlyStopping(monitor='loss', mode='min', verbose=1, patience=250)
mc = ModelCheckpoint(os.path.join(model_dir, 'model.h5'),
                     monitor='accuracy', mode='max', verbose=1, save_best_only=True)

history = model.fit(([X_mmw_rD, X_mmw_rA]), Y,
                    epochs=1800,
                    batch_size=16, callbacks=[es, mc], verbose=1, )
