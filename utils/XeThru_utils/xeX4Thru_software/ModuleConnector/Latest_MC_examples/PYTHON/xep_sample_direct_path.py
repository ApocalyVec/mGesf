#!/usr/bin/env python
""" \example xep_sample_direct_path.py

Latest examples is located at https://github.com/xethru/XeThru_ModuleConnector_Examples or https://dev.azure.com/xethru/XeThruApps/_git/XeThru_ModuleConnector_Examples.

# Target module:
# X4M200
# X4M300
# X4M03(XEP)

# Introduction: This is an example showing how to sample the direct path pulse and generates a similar pulse from a sine and a Gaussian envelope.

Original thread:
https://www.xethru.com/community/threads/radar-pulse-shape.329/#post-1604

# prerequisite:
# ModuleConnector python lib is installed, check XeThruSensorsIntroduction application note to get detail
# xt_modules_print_info.py should be in the same folder


"""

from __future__ import print_function, division

import matplotlib.pyplot as plt
from matplotlib import mlab
import numpy as np

import pymoduleconnector
from pymoduleconnector.extras.auto import auto
from scipy import interpolate

device_name = auto()[0]
# print_module_info(device_name)
mc = pymoduleconnector.ModuleConnector(device_name)

# Assume an X4M300/X4M200 module and try to enter XEP mode
app = mc.get_x4m300()
# Stop running application and set module in manual mode.
try:
    app.set_sensor_mode(0x13, 0)  # Make sure no profile is running.
except RuntimeError:
    # Profile not running, OK
    pass

try:
    app.set_sensor_mode(0x12, 0)  # Manual mode.
except RuntimeError:
    # Maybe running XEP firmware only?
    pass


xep = mc.get_xep()
# Set full DAC range
xep.x4driver_set_dac_min(0)
xep.x4driver_set_dac_max(2047)

# Set integration
xep.x4driver_set_iterations(16)
xep.x4driver_set_pulses_per_step(26)
xep.x4driver_set_frame_area(-1, 2)

# Sample a frame
xep.x4driver_set_fps(1)
d = xep.read_message_data_float()
frame = np.array(d.data)
xep.x4driver_set_fps(0)


fig = plt.figure(figsize=(16, 8))
fs = 23.328e9
nbins = len(frame)
x = np.linspace(0, (nbins-1)/fs, nbins)

# Calculate center frequency as the mean of -10 dB fl and fh
pxx, freqs = mlab.psd(frame, Fs=fs)
pxxdb = 10*np.log10(pxx)
arg10db = np.argwhere(pxxdb > (pxxdb.max()-10))
fl, fh = freqs[arg10db[0][0]], freqs[arg10db[-1][0]]
fc = (fl+fh)/2


# Pulse generator
# Pulse duration
bw = 1.4e9
#tau = 1/(pi*bw*sqrt(log10(e)))
tau = 340e-12
# Sampler
# Sampling rate
fs2 = fs*2

# delay to pulse
t0 = 3.64e-9

# Time array
t = np.linspace(0, (nbins-1)/fs2, nbins)

# Synthesize frames
frame_gen = np.exp(-((t-t0)**2)/(2*tau**2)) * np.cos(2 * np.pi * fc * (t - t0))

# Interpolate X4 frame
tck_1 = interpolate.splrep(x, frame)
frame_interp = interpolate.splev(t, tck_1, der=0)

frame_gen *= frame_interp.max()

# Plot frames
ax = fig.add_subplot(311)
ax.plot(x*1e9, frame, '-x', label='X4 pulse')
ax.plot(t*1e9, frame_interp, '-r', label='X4 pulse, interpolated')
ax.grid()
ax.set_xlim(ax.get_xlim()[0], t[-1]*1e9)
ax.set_xlabel("Time (ns)")
ax.set_ylabel("Normalized amplitude")
ax.legend()
ax.set_title("X4 sampled data")

ax = fig.add_subplot(312)
ax.plot(t*1e9, frame_gen, '-x', label='Generated pulse')
ax.plot(t*1e9, frame_interp, 'r', label='X4 pulse, interpolated')
ax.grid()
ax.set_xlabel("Time (ns)")
ax.set_ylabel("Normalized amplitude")
ax.set_xlim(ax.get_xlim()[0], t[-1]*1e9)
ax.legend()
ax.set_title("Generated and interpolated X4 pulse")

ax = fig.add_subplot(313)
ax.psd(frame_gen, Fs=fs2/1e9, label="Generated pulse")
ax.psd(frame_interp, Fs=fs2/1e9, label="X4 pulse, interpolated", color='r')
ax.set_xlim(0, 12)
ax.set_ylim(-84, -20)
ax.set_ylabel("PSD (Normalized)")
ax.set_xlabel("Frequency (GHz)")
ax.legend()
ax.set_title("PSD of sampled and generated pulse")

fig.suptitle("Sampled and generated X4 pulse in time and frequency domain", y=1)

fig.tight_layout()
fig.savefig("xep_sample_direct_path.png")

plt.show()
