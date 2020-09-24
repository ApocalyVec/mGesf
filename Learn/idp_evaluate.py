# temporal probability

import matplotlib
import matplotlib.pyplot as plt
import numpy as np
from keras.engine.saving import load_model
from sklearn.preprocessing import OneHotEncoder

from Learn.data_in import idp_preprocess_legacy, resolve_points_per_sample
from utils.data_utils import prepare_x, moving_average

########################################################################################################################
# idp_data_dir = ['../data/idp-ABCDE-rpt10', '../data/idp-ABCDE-rpt2']
# num_repeats = [10, 2]
# classes = ['A', 'B', 'C', 'D', 'E']
########################################################################################################################

# idp_data_dir = ['../data/idp-FGHIJ-rpt10']
# num_repeats = [10]
# classes = ['F', 'G', 'H', 'I', 'L']
########################################################################################################################
# idp_data_dir = ['../data/idp-KLMNO-rpt10']
# num_repeats = [10]
# sample_classes = [['K', 'L', 'M', 'N', 'O']]
# classes = ['K', 'L', 'M', 'N', 'O']
########################################################################################################################
# idp_data_dir = ['../data/idp-PQRST-rpt10']
# num_repeats = [10]
# sample_classes = [['P', 'Q', 'R', 'S', 'T']]
# classes = ['P', 'Q', 'R', 'S', 'T']
########################################################################################################################
# idp_data_dir = ['../data/idp-UVWXY-rpt10']
# num_repeats = [10]
# sample_classes = [['U', 'V', 'W', 'X', 'Y']]
# classes = ['U', 'V', 'W', 'X', 'Y']
########################################################################################################################
# idp_data_dir = ['../data/idp-ZSpcBspcEnt-rpt10']
# num_repeats = [10]
# sample_classes = [['Z', 'Spc', 'Bspc', 'Ent']]
# classes = ['Z', 'Spc', 'Bspc', 'Ent']
########################################################################################################################

# idp_data_dir = ['/Users/Leo/Documents/data/idp_29/data/idp-ABCDE-rpt10',
#                 '/Users/Leo/Documents/data/idp_29/data/idp-ABCDE-rpt2',
#                 '/Users/Leo/Documents/data/idp_29/data/idp-FGHIJ-rpt10',
#                 '/Users/Leo/Documents/data/idp_29/data/idp-KLMNO-rpt10',
#                 '/Users/Leo/Documents/data/idp_29/data/idp-PQRST-rpt10',
#                 '/Users/Leo/Documents/data/idp_29/data/idp-UVWXY-rpt10',
#                 '/Users/Leo/Documents/data/idp_29/data/idp-ZSpcBspcEnt-rpt10']
idp_data_dir = ['D:\PycharmProjects\mGesf\data/idp-ABCDE-rpt10',
                'D:\PycharmProjects\mGesf\data/idp-ABCDE-rpt2',
                'D:\PycharmProjects\mGesf\data/idp-FGHIJ-rpt10',
                'D:\PycharmProjects\mGesf\data/idp-KLMNO-rpt10',
                'D:\PycharmProjects\mGesf\data/idp-PQRST-rpt10',
                'D:\PycharmProjects\mGesf\data/idp-UVWXY-rpt10',
                'D:\PycharmProjects\mGesf\data/idp-ZSpcBspcEnt-rpt10']
num_repeats = [10, 2, 10, 10, 10, 10, 10]
sample_classes = [['A', 'B', 'C', 'D', 'E'],
                  ['A', 'B', 'C', 'D', 'E'],  # some of the ABCDE data are repeated twice
                  ['F', 'G', 'H', 'I', 'J'],
                  ['K', 'L', 'M', 'N', 'O'],
                  ['P', 'Q', 'R', 'S', 'T'],
                  ['U', 'V', 'W', 'X', 'Y'],
                  ['Z', 'Spc', 'Bspc', 'Ent']]
classes = ['A', 'B', 'C', 'D', 'E',
           'F', 'G', 'H', 'I', 'J',
           'K', 'L', 'M', 'N', 'O',
           'P', 'Q', 'R', 'S', 'T',
           'U', 'V', 'W', 'X', 'Y',
           'Z', 'Spc', 'Bspc', 'Ent']

assert len(idp_data_dir) == len(num_repeats) == len(sample_classes)  # check the consistency of zip variables
assert set(classes) == set([item for sublist in sample_classes for item in sublist])  # check categorical consistency
########################################################################################################################

interval_duration = 4.0  # how long does one writing take
period = 33  # ms

# classes = set([item for sublist in sample_classes for item in sublist])  # reduce to categorical classes
ls_dicts = \
    [idp_preprocess_legacy(dr, interval_duration, classes=cs, num_repeat=nr, period=period)
     for dr, nr, cs in zip(idp_data_dir, num_repeats, sample_classes)]
points_per_sample = int(resolve_points_per_sample(period, interval_duration))

# create input features
Y = []
X_mmw_rD = []
X_mmw_rA = []

# add to x and y
for lsd in ls_dicts:
    for key, value in lsd.items():
        X_mmw_rD += [d for d in value['mmw']['range_doppler']]
        X_mmw_rA += [a for a in value['mmw']['range_azi']]
        Y += [key for i in range(value['mmw']['range_doppler'].shape[0])]
        pass

X_mmw_rD = np.asarray(X_mmw_rD)
X_mmw_rA = np.asarray(X_mmw_rA)
Y = np.asarray(Y)

encoder = OneHotEncoder(categories='auto')
Y = encoder.fit_transform(np.expand_dims(Y, axis=1)).toarray()

idp_model = load_model('E:\mgesf_backup\models\idp\idp_29_2020-05-04_03-24-10.425555.h5')

# make a contiuous temporal sequence A, B, C, D, E
# TODO have this followed by a void character
# key_indices = [0, 160, 320, 480, 640]  # A, B, C, D, E
sequence = np.reshape(np.array(['H', 'E', 'L', 'L', 'O', 'Spc', 'W', 'O', 'R', 'L', 'D', 'Ent']), newshape=(-1, 1))
valid_indices = np.argmax(encoder.transform(sequence).toarray(), axis=1)
index_class_dict = dict([(index, clss[0]) for index, clss in zip(valid_indices, sequence)])

H_index = np.where(np.all(Y == encoder.transform([['H']]).toarray(), axis=1))[0][0]
E_index = np.where(np.all(Y == encoder.transform([['E']]).toarray(), axis=1))[0][0]
L_index = np.where(np.all(Y == encoder.transform([['L']]).toarray(), axis=1))[0][0]
O_index = np.where(np.all(Y == encoder.transform([['O']]).toarray(), axis=1))[0][0]
Spc_index = np.where(np.all(Y == encoder.transform([['Spc']]).toarray(), axis=1))[0][0]
W_index = np.where(np.all(Y == encoder.transform([['W']]).toarray(), axis=1))[0][0]
R_index = np.where(np.all(Y == encoder.transform([['R']]).toarray(), axis=1))[0][0]
D_index = np.where(np.all(Y == encoder.transform([['D']]).toarray(), axis=1))[0][0]
Ent_index = np.where(np.all(Y == encoder.transform([['Ent']]).toarray(), axis=1))[0][0]

key_indices = np.array([H_index, H_index + 1, E_index, L_index, L_index + 1, O_index,
                        Spc_index, W_index + 1, O_index + 1, R_index, L_index + 2, D_index, Ent_index, Ent_index + 1])  # H, E, L, L, O, spc, W, O, R, L, D
ys = np.array([Y[ki] for ki in key_indices])
print('Working with sequence: ' + str(encoder.inverse_transform(ys)))

rA_seq = np.array([X_mmw_rA[i] for i in key_indices])
rA_seq = np.reshape(rA_seq,
                    newshape=[-1] + list(rA_seq.shape[2:]))  # flatten the sample dimension to create temporal sequence
rD_seq = np.array([X_mmw_rD[i] for i in key_indices])
rD_seq = np.reshape(rD_seq,
                    newshape=[-1] + list(rD_seq.shape[2:]))  # flatten the sample dimension to create temporal sequence

# sample from the temporal sequence
rA_samples = prepare_x(rA_seq, window_size=121, stride=1)
rD_samples = prepare_x(rD_seq, window_size=121, stride=1)

y_pred = idp_model.predict([rD_samples, rA_samples], batch_size=32)
y_pred = y_pred[60:len(y_pred) - 60]

# plottings
matplotlib.rcParams.update({'font.size': 14})
plt.figure(figsize=(20, 6))
is_plotted_others = False

for i, col in enumerate(np.transpose(y_pred)):
    if i in valid_indices:
        plt.plot(moving_average(col, n=16), label='Predicted gesture: ' + index_class_dict[i], linewidth=3)
    else:
        plt.plot(moving_average(col, n=16), c='gray', label='Gestures for other chars') if not is_plotted_others else plt.plot(col,                                                                                                        c='gray')
        is_plotted_others = True

# plot char separation lines
for i in range(1, len(key_indices) - 2):
    plt.axvline(x=121 * i, c='0.3', linewidth=5)

# debouncer_frame_threshold = 30
# debouncer_prob_threshold = 0.9
# debouncer = [0] * len(classes)
# for i, frame_pred in enumerate(y_pred):
#     break_indices = np.argwhere(frame_pred > debouncer_prob_threshold)
#     for bi in break_indices:
#         bi = bi[0]
#         debouncer[bi] = debouncer[bi] + 1
#         if debouncer[bi] > debouncer_frame_threshold:
#             plt.plot([i], [0.9], 'bo')
#             plt.text(i, 0.95, index_class_dict[bi] + 'Detected ', fontsize=12, c='blue')
#             debouncer = [0] * len(classes)

# plt.legend(loc=4)
plt.xlabel('Frames (30 frames per second)')
plt.ylabel('Probability of class prediction')
# plt.title('Temporal Probability cross a Continuous Sequence of "A, B, C, D, E"')
plt.title('Temporal Probability cross a Continuous Sequence of "H, E, L, L, O, Space, W, O, R, L, D"')
plt.title('Temporal Probability cross a Continuous Sequence of "H, E, L, L, O, Space, W, O, R, L, D", with Debouncer Detection')
plt.show()
