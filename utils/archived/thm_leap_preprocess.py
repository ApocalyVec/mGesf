import os
import pickle

import numpy as np
from utils.path_utils import generate_path_thm_leap, thm_leap_preprocess

date = 121319
label_path = 'D:/dataset_thm_leap_' + str(date) + '/label.p'
dataset_path = 'D:/dataset_thm_leap_' + str(date)

specimen_list = [

    # generate_path_thm_leap('zl', 4), # bad: timestamp on radar is off by 370 sec or so
    # generate_path_thm_leap('zl', 5),
    #
    # generate_path_thm_leap('zy', 3),
    # generate_path_thm_leap('zy', 4),
    # generate_path_thm_leap('zy', 5),
    #
    # generate_path_thm_leap('sd', 3),
    # generate_path_thm_leap('sd', 4),
    # generate_path_thm_leap('sd', 5),
    #
    #
    # generate_path_thm_leap('am', 3),
    # generate_path_thm_leap('am', 4),
    # generate_path_thm_leap('am', 5),
    #
    # generate_path_thm_leap('ya', 3),
    # generate_path_thm_leap('ya', 4),
    # generate_path_thm_leap('ya', 5),

    # generate_path_thm_leap('zl', 0),
    # generate_path_thm_leap('zl', 1),
]

specimen_list = [generate_path_thm_leap('zl', x, date=date) for x in range(14)]

timestep = 3

for i, path in enumerate(specimen_list):
    # generate orignial data
    print('Processing specimen #' + str(i) + ' ' + str(path[0]) + '__________________________________')
    thm_leap_preprocess(path, buffer_size=timestep, dataset_path=dataset_path, label_path=label_path)
    # thm_leap_preprocess(path, augmentation=['trans'], buffer_size=timestep, dataset_path=dataset_path, label_path=label_path)
    # thm_preprocess(path, augmentation=['scale'], buffer_size=timestep)
    # thm_preprocess(path, augmentation=['rot'], buffer_size=timestep)

# just plot the thing
# for i, path in enumerate(specimen_list):
#     # generate orignial data
#     print('Processing specimen #' + str(i) + '__________________________________')
#     idp_preprocess(path, is_plot=True)