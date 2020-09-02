import datetime
import pickle

import numpy as np
import os
import matplotlib

import matplotlib.pyplot as plt

from sklearn.model_selection import train_test_split
from sklearn.preprocessing import OneHotEncoder
from tensorflow.python.keras.callbacks import EarlyStopping, ModelCheckpoint

from Learn.data_in import idp_preprocess_legacy, resolve_points_per_sample, idp_preprocess_convert_legacy
from config import rd_shape, ra_shape
from utils.data_utils import plot_confusion_matrix

idp_data_dir = ['F:/data/mGesf/050120_zl_legacy/idp-ABCDE-rpt10',
                'F:/data/mGesf/050120_zl_legacy/idp-ABCDE-rpt2',
                'F:/data/mGesf/050120_zl_legacy/idp-FGHIJ-rpt10',
                'F:/data/mGesf/050120_zl_legacy/idp-KLMNO-rpt10',
                'F:/data/mGesf/050120_zl_legacy/idp-PQRST-rpt10',
                'F:/data/mGesf/050120_zl_legacy/idp-UVWXY-rpt10',
                'F:/data/mGesf/050120_zl_legacy/idp-ZSpcBspcEnt-rpt10'
                ]

num_repeats = [10, 2,
               10, 10, 10, 10, 10
               ]
sample_classes = [['A', 'B', 'C', 'D', 'E'],
                  ['A', 'B', 'C', 'D', 'E'],  # some of the ABCDE data are repeated twice
                  ['F', 'G', 'H', 'I', 'J'],
                  ['K', 'L', 'M', 'N', 'O'],
                  ['P', 'Q', 'R', 'S', 'T'],
                  ['U', 'V', 'W', 'X', 'Y'],
                  ['Z', 'Spc', 'Bspc', 'Ent']
                  ]
classes = ['A', 'B', 'C', 'D', 'E',
           'F', 'G', 'H', 'I', 'J',
           'K', 'L', 'M', 'N', 'O',
           'P', 'Q', 'R', 'S', 'T',
           'U', 'V', 'W', 'X', 'Y',
           'Z', 'Spc', 'Bspc', 'Ent'
           ]

output = 'D:/ResearchProjects/mGesf/data/050120_zl'
subject_name = 'zl'

ls_dicts = \
    [idp_preprocess_convert_legacy(dr, output_dir=output, classes=cs, num_repeat=nr, subject_name=subject_name)
     for dr, nr, cs in zip(idp_data_dir, num_repeats, sample_classes)]