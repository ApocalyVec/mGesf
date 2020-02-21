import numpy as np
from scipy.spatial import distance

import pickle

datapath = 'data/f_data_thm_zl_0/f_data_points.p'

frames = pickle.load(open(datapath, 'rb'))
frame_wo_ts = [x[1] for x in frames]
f = frame_wo_ts[86]

v = [x[:, 3] for x in frame_wo_ts if len(x) > 0]
v_m = [max(x) for x in v]

r_res = 0.04033
d_res = 0.13659
r_lim = 10
d_lim = 5

def dtp_to_profile(dtp):
    rd_profile = np.zeros((r_lim, d_lim))
    ranges = [distance.euclidean((p[0], p[1], p[2]), (0., 0., 0.)) for p in dtp]
    dopplers = dtp[:, 3]

    r_coords = [round(r / r_res) for r in ranges]
    d_coord = [round(d / d_res) + d_lim for d in dopplers]
    pass


profile = dtp_to_profile(f)
