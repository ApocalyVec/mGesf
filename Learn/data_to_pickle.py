import pickle

import numpy as np
from sklearn.preprocessing import OneHotEncoder

from Learn.data_in import resolve_points_per_sample
from utils.data_utils import load_idp

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
encoder.categories_[0] = np.array(idp_complete_classes)
# 090120_all
X_dict, Y = load_idp('C:/Users/HaowenWeiJohn/Desktop/IndexPen_Data/2020_Data/zl_5800',
                     sensor_feature_dict=sensor_feature_dict,
                     complete_class=idp_complete_classes, encoder=encoder,
                     sensor_sample_points_dict=sensor_sample_points_dict)

save_data_dir = 'C:/Users/HaowenWeiJohn/Desktop/IndexPen_Data/2020_Data/X_dict_Y_Encoder/zl_5800_corrupt_frame_removal_test'

with open(save_data_dir, 'wb') as f:
    pickle.dump([X_dict, Y, encoder], f)
