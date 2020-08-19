import re
import time
import config
import numpy as np


def camel_case_split(str):
    return re.findall(r'[A-Z](?:[a-z]+|[A-Z]*(?=[A-Z]|$))', str)


def record_mmw_frame(data_dict, buffer):
    ts = time.time()
    try:
        assert data_dict['range_doppler'].shape == config.rd_shape
        assert data_dict['range_azi'].shape == config.ra_shape
    except AssertionError:
        print('Invalid data shape at ' + str(ts) + ', discarding frame.')
        return
    finally:
        buffer['mmw']['timestamps'].append(ts)
        # expand spectrogram dimension for channel_first
        buffer['mmw']['range_doppler'].append(np.expand_dims(data_dict['range_doppler'], axis=0))
        buffer['mmw']['range_azi'].append(np.expand_dims(data_dict['range_azi'], axis=0))
        buffer['mmw']['detected_points'].append(data_dict['pts'])