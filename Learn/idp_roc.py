from sklearn.model_selection import train_test_split
from sklearn.preprocessing import OneHotEncoder
from tensorflow.python.keras.models import load_model

from Learn.data_in import resolve_points_per_sample
from utils.data_utils import load_idp_new_and_legacy, plot_confusion_matrix
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

X_mmw_rD, X_mmw_rA, Y = load_idp_new_and_legacy('E:/data/mGesf/090120',
                                                sensor_feature_dict=sensor_feature_dict,
                                                complete_class=idp_complete_classes, encoder=encoder,
                                                sensor_sample_points_dict=sensor_sample_points_dict,
                                                legacy_root='E:/data/mGesf/050120_zl_legacy'
                                                )

X_mmw_rD = (X_mmw_rD - np.min(X_mmw_rD)) / (np.max(X_mmw_rD) - np.min(X_mmw_rD))
X_mmw_rA = (X_mmw_rA - np.min(X_mmw_rA)) / (np.max(X_mmw_rA) - np.min(X_mmw_rA))

X_mmw_rD_train, X_mmw_rD_test, Y_train, Y_test = train_test_split(X_mmw_rD, Y, test_size=0.20, random_state=3,
                                                                  shuffle=True)
X_mmw_rA_train, X_mmw_rA_test, Y_train, Y_test = train_test_split(X_mmw_rA, Y, test_size=0.20, random_state=3,
                                                                  shuffle=True)

from pathlib import Path
import os
model_path = 'D:/PcProjects/mGesf/models/idp_all/2020-09-04_05-41-48.712257.h5'
model = load_model(model_path)

model.evaluate(x=[X_mmw_rD_test, X_mmw_rA_test], y=Y_test, batch_size=32)

y_pred = model.predict([X_mmw_rD_test, X_mmw_rA_test], batch_size=32)
plot_confusion_matrix(Y_test.argmax(axis=1), y_pred.argmax(axis=1), classes=np.array(idp_complete_classes),
                      normalize=True, title='IndexPen Confusion Matrix')
plt.show()