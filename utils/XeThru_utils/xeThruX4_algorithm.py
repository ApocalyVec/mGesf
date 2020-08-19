import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import time

from scipy.signal import butter, lfilter, filtfilt


def xep_rf_frame_downconversion(rf_frame, fc_index, fs):
    if fc_index == 3:
        fc = 7.29e9
    else:
        fc = 8.748e9

    csine = np.exp(-1j * fc / fs * 2 * np.pi * np.arange(len(rf_frame)))


    cframe = rf_frame * csine

    # dc_frame = 0
    return cframe



def xep_raw_data_phase():
    return


def xep_doppler():
    return


# filters


def xep_rf_frame_clutter_removal(rf_frame, clutter):
    return


def butter_lowpass_filter(data, cutoff, fs, order):
    nyq = 0.5*fs
    normal_cutoff = cutoff / nyq
    # Get the filter coefficients
    b, a = butter(order, normal_cutoff, btype='low')
    y = filtfilt(b, a, data)
    return y

