import os

from sklearn.model_selection import train_test_split
from sklearn.preprocessing import OneHotEncoder
from tensorflow.python.keras.models import load_model
import pickle
from Learn.data_in import resolve_points_per_sample
from utils.data_utils import load_idp_new_and_legacy, prepare_x, index_to_class
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

experiment_root = 'E:/data/mGesf/091020/'
scenario = 'Watch'
subjects = ['ag', 'hw']
test_cases = ['A-Act', 'Nois_Act_HelloWorld_Act_Nois', 'Pangram']
reload_data = False

data_suffix = '_data.mgesf'
label_suffix = '_label.mgesf'

sensor_feature_dict = {'mmw': ('range_doppler', 'range_azi')}
sensor_period_dict = {'mmw': 33.45}  # period in milliseconds
input_interval = 4.0
encoder = OneHotEncoder(categories='auto')
encoder.fit(np.reshape(idp_complete_classes, (-1, 1)))
sensor_sample_points_dict = dict(
    [(key, (resolve_points_per_sample(value, input_interval))) for key, value in sensor_period_dict.items()])
print('loading original data for normalization')
rD_max, rD_min = pickle.load(open('D:/PcProjects/mGesf/data/rD_max.p', 'rb')), pickle.load(
    open('D:/PcProjects/mGesf/data/rD_min.p', 'rb'))
rA_max, rA_min = pickle.load(open('D:/PcProjects/mGesf/data/rA_max.p', 'rb')), pickle.load(
    open('D:/PcProjects/mGesf/data/rA_min.p', 'rb'))

print('loading model')
model_dir = os.path.join(experiment_root, '0' + scenario, 'model')
model = load_model(os.path.join(model_dir, 'model.h5'))
data_dir = os.path.join(experiment_root, '0' + scenario)
if reload_data:
    X_mmw_rD, X_mmw_rA, Y = load_idp_new_and_legacy(data_dir,
                                                    sensor_feature_dict=sensor_feature_dict,
                                                    complete_class=idp_complete_classes, encoder=encoder,
                                                    sensor_sample_points_dict=sensor_sample_points_dict
                                                    )
    X_mmw_rD = (X_mmw_rD - rD_min) / (rD_max - rD_min)
    X_mmw_rA = (X_mmw_rA - rA_min) / (rA_max - rA_min)

    print('Checking sanity')
    print(model.evaluate(x=[X_mmw_rD, X_mmw_rA], y=Y, batch_size=32))


# distance test ######################################################
# def resolve_test_case(data_path, label_path, idp_complete_classes, rD_min, rD_max, rA_min, rA_max, debouncer_frame_threshold = 30, debouncer_prob_threshold = 0.9):
#     data = pickle.load(open(data_path, 'rb'))
#     label = pickle.load(open(label_path, 'rb'))
#
#     sequence = np.reshape(np.array(label), newshape=(-1, 1))
#     sequence = np.array([[x[0]] for x in sequence if x[0] in idp_complete_classes])
#     valid_indices = np.argmax(encoder.transform(sequence).toarray(), axis=1)
#     index_class_dict = dict([(index, clss[0]) for index, clss in zip(valid_indices, sequence)])
#
#     rD_seq = np.array(data['mmw']['range_doppler'])
#     rA_seq = np.array(data['mmw']['range_azi'])
#
#     rD_seq = (rD_seq - rD_min) / (rD_max - rD_min)
#     rA_seq = (rA_seq - rA_min) / (rA_max - rA_min)
#
#     rA_samples = prepare_x(rA_seq, window_size=120, stride=1)
#     rD_samples = prepare_x(rD_seq, window_size=120, stride=1)
#
#     rA_samples = prepare_x(rA_seq, window_size=120, stride=1)
#     rD_samples = prepare_x(rD_seq, window_size=120, stride=1)
#
#     print('predicting on samples')
#     y_pred = model.predict([rD_samples, rA_samples], batch_size=32)
#
#     debouncer = [0] * len(idp_complete_classes)
#     for i, frame_pred in enumerate(y_pred):
#         break_indices = np.argwhere(frame_pred > debouncer_prob_threshold)
#         for bi in break_indices:
#             bi = bi[0]
#             debouncer[bi] = debouncer[bi] + 1
#             if debouncer[bi] > debouncer_frame_threshold:
#                 plt.plot([i], [0.9], 'bo')
#                 plt.text(i, 0.95, index_class_dict[bi] + 'Detected ', fontsize=12, c='blue')
#                 debouncer = [0] * len(idp_complete_classes)
#
#
# subject_folders = [os.path.join(experiment_root, s + '_Test', )for s in subjects]
#
# for i, sf in enumerate(subject_folders):
#     for j, tc in enumerate(test_cases):
#         subject_test_case_data_dir = os.path.join(sf, tc)
#         for k, data_path in enumerate(subject_test_case_data_dir):
#             print('loading file ' + str(k) + ' of ' + str(len(os.listdir(subject_test_case_data_dir))) + ', file name is ' + data_path)
#             if data_path.endswith(data_suffix):
#                 label_path = data_path.replace(data_suffix, '') + label_suffix
#                 resolve_test_case(data_path, label_path, idp_complete_classes, rD_min, rD_max, rA_min, rA_max)
# individual sequence test ######################################################


print('loading samples')
# Hello World HW Lab
# data = pickle.load(open('E:/data/mGesf/091020/hw_Test/Lab/Nois_Act_HelloWorld_Act_Nois/Sep-05-2020-23-41-40_hw_data.mgesf', 'rb'))
# label = pickle.load(open('E:/data/mGesf/091020/hw_Test/Lab/Nois_Act_HelloWorld_Act_Nois/Sep-05-2020-23-41-40_hw_label.mgesf', 'rb'))
# Hello World HW Cloth
# data = pickle.load(open('E:/data/mGesf/091020/hw_Test/Cloth/Nois_Act_HelloWorld_Act_Nois/Sep-06-2020-00-07-29_hw_data.mgesf', 'rb'))
# label = pickle.load(open('E:/data/mGesf/091020/hw_Test/Cloth/Nois_Act_HelloWorld_Act_Nois/Sep-06-2020-00-07-29_hw_label.mgesf', 'rb'))
# Hello World HW Watch
data = pickle.load(open('E:/data/mGesf/091020/hw_Test/Watch/Nois_Act_HelloWorld_Act_Nois/Sep-11-2020-15-56-00_hw_data.mgesf', 'rb'))
label = pickle.load(open('E:/data/mGesf/091020/hw_Test/Watch/Nois_Act_HelloWorld_Act_Nois/Sep-11-2020-15-56-00_hw_label.mgesf', 'rb'))

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
y_pred = model.predict([rD_samples, rA_samples], batch_size=32)

# plottings
print('plotting')
matplotlib.rcParams.update({'font.size': 16})
plt.figure(figsize=(20, 6))
is_plotted_others = False
for i, col in enumerate(np.transpose(y_pred)):
    if i in index_class_dict.keys():
        plt.plot(col, label='Predicted gesture: ' + index_class_dict[i], linewidth=3)
    else:
        plt.plot(col, c='gray', label='Gestures for other chars') if not is_plotted_others else plt.plot(col, c='gray')
        is_plotted_others = True

# plot char separation lines
# for i in range(1, len(index_class_dict) + 1):
#     plt.axvline(x=120 * i - 60, c='0.3', linewidth=5)

debouncer_frame_threshold = 40
debouncer_prob_threshold = 0.9
relax_period = 45
debouncer = [0] * len(idp_complete_classes)
relax_counter = 0
detection_sequence = []
jerk = True
for i, frame_pred in enumerate(y_pred):
    if relax_counter == 30:
        break_indices = np.argwhere(frame_pred > debouncer_prob_threshold)
        for bi in break_indices:
            bi = bi[0]
            debouncer[bi] = debouncer[bi] + 1
            if debouncer[bi] > debouncer_frame_threshold:
                plt.plot([i], [0.9], 'bo')
                plt.text(i - 20, 0.95 if jerk else 0.85, index_to_class(bi, encoder) + ' Detected ', weight="bold", fontsize=12, c='blue')
                jerk = not jerk
                detection_sequence.append(index_to_class(bi, encoder))
                debouncer = [0] * len(idp_complete_classes)
                relax_counter = 0
    else:
        relax_counter = min(relax_counter + 1, 30)

plt.ylabel('Predicted probabilities of gestures')
plt.xlabel('Frames (at 30 fps)')
plt.legend(loc=4)
plt.show()
