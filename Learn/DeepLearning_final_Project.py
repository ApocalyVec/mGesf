import datetime

from sklearn.model_selection import train_test_split
from sklearn.preprocessing import OneHotEncoder
import numpy as np
from tensorflow.python.keras.models import load_model

from Learn.data_in import resolve_points_per_sample
from utils.data_utils import load_idp
from utils.learn_utils import make_model, make_model_simple
from tensorflow.python.keras.callbacks import EarlyStopping, ModelCheckpoint

import matplotlib.pyplot as plt


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
sensor_sample_points_dict = dict([(key, (resolve_points_per_sample(value, input_interval))) for key, value in sensor_period_dict.items()])

encoder = OneHotEncoder(categories='auto')
encoder.fit(np.reshape(idp_complete_classes, (-1, 1)))
X_dict, Y = load_idp('C:/Users/HaowenWeiJohn/Desktop/mGesf_data_test',
                     sensor_feature_dict=sensor_feature_dict,
                     complete_class=idp_complete_classes, encoder=encoder, sensor_sample_points_dict=sensor_sample_points_dict)

#####################################################################################
X_mmw_rD = X_dict['range_doppler']
X_mmw_rA = X_dict['range_azi']

# min-max normalize
X_mmw_rD = (X_mmw_rD - np.min(X_mmw_rD)) / (np.max(X_mmw_rD) - np.min(X_mmw_rD))
X_mmw_rA = (X_mmw_rA - np.min(X_mmw_rA)) / (np.max(X_mmw_rA) - np.min(X_mmw_rA))

# z normalize
# X_mmw_rD_zScore = (X_mmw_rD - np.mean(X_mmw_rD))/np.std(X_mmw_rD)
# X_mmw_rA_zScore = (X_mmw_rA - np.mean(X_mmw_rA))/np.std(X_mmw_rA)

X_mmw_rD_train, X_mmw_rD_test, Y_train, Y_test = train_test_split(X_mmw_rD, Y, test_size=0.20, random_state=3,
                                                                  shuffle=True)
X_mmw_rA_train, X_mmw_rA_test, Y_train, Y_test = train_test_split(X_mmw_rA, Y, test_size=0.20, random_state=3,
                                                                  shuffle=True)
print("John")
