# temporal probability
import pickle

import matplotlib
import matplotlib.pyplot as plt
import numpy as np
from keras.engine.saving import load_model

from utils.data_utils import prepare_x, moving_average

idp_model = load_model('E:\mgesf_backup\models\idp\idp_29_2020-05-04_03-24-10.425555.h5')
encoder = pickle.load(open('D:\PcProjects\mGesf\data\data_may\encoder_082020.p', 'rb'))
classes = ['A', 'B', 'C', 'D', 'E',
           'F', 'G', 'H', 'I', 'J',
           'K', 'L', 'M', 'N', 'O',
           'P', 'Q', 'R', 'S', 'T',
           'U', 'V', 'W', 'X', 'Y',
           'Z', 'Spc', 'Bspc', 'Ent']

sequence = np.reshape(np.array(['A', 'B', 'C', 'D', 'E']), newshape=(-1, 1))
# sequence = np.reshape(np.array(['H', 'E', 'L', 'L', 'O', 'Spc', 'W', 'O', 'R', 'L', 'D', 'Ent']), newshape=(-1, 1))
valid_indices = np.argmax(encoder.transform(sequence).toarray(), axis=1)
index_class_dict = dict([(index, clss[0]) for index, clss in zip(valid_indices, sequence)])

data_path = 'D:\PcProjects\mGesf\data\data_may\Aug-20-2020-15-48-29_data.mgesf'
data = pickle.load(open(data_path, 'rb'))

rA_seq = np.array(data['mmw']['range_azi'])
rD_seq = np.array(data['mmw']['range_doppler'])

rA_samples = prepare_x(rA_seq, window_size=121, stride=1)
rD_samples = prepare_x(rD_seq, window_size=121, stride=1)

y_pred = idp_model.predict([rD_samples, rA_samples], batch_size=32)

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
for i in range(1, len(valid_indices) - 2):
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

plt.legend(loc=4)
plt.xlabel('Frames (30 frames per second)')
plt.ylabel('Probability of class prediction')
plt.title('Temporal Probability cross a Continuous Sequence of "H, E, L, L, O, Space, W, O, R, L, D"')
# plt.title('Temporal Probability cross a Continuous Sequence of "H, E, L, L, O, Space, W, O, R, L, D", with Debouncer Detection')
plt.show()
