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

reload_data = True

print('loading model')
idp_model = load_model('D:/PcProjects/mGesf/models/idp_all/2020-09-04_05-41-48.712257.h5')
if reload_data:
    X_mmw_rD, X_mmw_rA, Y = load_idp_new_and_legacy('E:/data/mGesf/090120',
                                                    sensor_feature_dict=sensor_feature_dict,
                                                    complete_class=idp_complete_classes, encoder=encoder,
                                                    sensor_sample_points_dict=sensor_sample_points_dict,
                                                    legacy_root='E:/data/mGesf/050120_zl_legacy'
                                                    )

    rD_max, rD_min = np.max(X_mmw_rD), np.min(X_mmw_rD)
    rA_max, rA_min = np.max(X_mmw_rA), np.min(X_mmw_rA)
    X_mmw_rD = (X_mmw_rD - rD_min) / (rD_max - rD_min)
    X_mmw_rA = (X_mmw_rA - rA_min) / (rA_max - rA_min)

    pickle.dump(rD_max, open('D:/PcProjects/mGesf/data/rD_max.p', 'wb'))
    pickle.dump(rD_min, open('D:/PcProjects/mGesf/data/rD_min.p', 'wb'))

    pickle.dump(rA_max, open('D:/PcProjects/mGesf/data/rA_max.p', 'wb'))
    pickle.dump(rA_min, open('D:/PcProjects/mGesf/data/rA_min.p', 'wb'))
    print('Checking sanity')
    idp_model.evaluate(x=[X_mmw_rD, X_mmw_rA], y=Y, batch_size=32)
    del X_mmw_rD, X_mmw_rA, Y
else:
    rD_max, rD_min = pickle.load(open('D:/PcProjects/mGesf/data/rD_max.p', 'rb')), pickle.load(open('D:/PcProjects/mGesf/data/rD_min.p', 'rb'))
    rA_max, rA_min = pickle.load(open('D:/PcProjects/mGesf/data/rA_max.p', 'rb')), pickle.load(open('D:/PcProjects/mGesf/data/rA_min.p', 'rb'))

print('loading samples')
data = pickle.load(open('E:/data/mGesf/091020/hw_Test/Lab/A-Act/Sep-03-2020-22-12-31_hw_data.mgesf', 'rb'))
label = pickle.load(open('E:/data/mGesf/091020/hw_Test/Lab/A-Act/Sep-03-2020-22-12-31_hw_label.mgesf', 'rb'))
sequence = np.reshape(np.array(label), newshape=(-1, 1))
# remove non-class sequence elements
sequence = np.array([[x[0]] for x in sequence if x[0] in idp_complete_classes])
valid_indices = np.argmax(encoder.transform(sequence).toarray(), axis=1)
index_class_dict = dict([(index, clss[0]) for index, clss in zip(valid_indices, sequence)])

rD_seq = np.array(data['mmw']['range_doppler'])
rA_seq = np.array(data['mmw']['range_azi'])

print('performaing normalization')
rD_seq = (rD_seq - rD_min) / (rD_max - rD_min)
rA_seq = (rA_seq - rA_min) / (rA_max - rA_min)

rA_samples = prepare_x(rA_seq, window_size=120, stride=1)
rD_samples = prepare_x(rD_seq, window_size=120, stride=1)

print('predicting on samples')
y_pred = idp_model.predict([rD_samples, rA_samples], batch_size=32)

# plottings
print('plotting')
matplotlib.rcParams.update({'font.size': 8})
plt.figure(figsize=(20, 6))
is_plotted_others = False
for i, col in enumerate(np.transpose(y_pred[:600])):
    if i in index_class_dict.keys():
        plt.plot(col, label='Predicted gesture: ' + index_class_dict[i], linewidth=1)
    else:
        plt.plot(col, c='gray', label='Gestures for other chars') if not is_plotted_others else plt.plot(col, c='gray')
        is_plotted_others = True

# for i, col in enumerate(np.transpose(y_pred)):
#     plt.plot(col, label='Predicted gesture: ' + str(i), linewidth=1)

# debouncer_frame_threshold = 30
# debouncer_prob_threshold = 0.9
# debouncer = [0] * len(idp_complete_classes)
# for i, frame_pred in enumerate(y_pred):
#     break_indices = np.argwhere(frame_pred > debouncer_prob_threshold)
#     for bi in break_indices:
#         bi = bi[0]
#         debouncer[bi] = debouncer[bi] + 1
#         if debouncer[bi] > debouncer_frame_threshold:
#             plt.plot([i], [0.9], 'bo')
#             plt.text(i, 0.95, index_class_dict[bi] + 'Detected ', fontsize=12, c='blue')
#             debouncer = [0] * len(idp_complete_classes)


plt.legend()
plt.show()
