#!/usr/bin/env python
# -*- coding: utf-8 -*-
""" \example X2M200_plot_respiration.py

#Target module: X2M200

#Introduction: This is an example of how to set up and read respiration messages from the
               X2M200 module with the ModuleConnector python wrapper.

#Command to run: "python X2M200_read_recording.py -d com8" or "python3 X2M200_read_recording.py -d com8"
                 change "com8" with your device name, using "--help" to see other options.
"""
from __future__ import print_function
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from collections import deque
from pymoduleconnector import ModuleConnector

def x2m200_respiration_message_example(device_name, detection_zone=(0.4,1)):
    """ X2M200 respiration message example.

    This example connects to a X2M200 device, loads the sleep profile,
    configures the sensor and plots the respiration message output.

    Parameters
    ----------
    * device_name : str
        Device name connection string, e.g Windows com4, linux /dev/ttyACM0

    * detection_zone : tuple, optional
        detection zone start and stop.

    Returns
    -------
    The initiated x2m200 object.
    """

    zstart = detection_zone[0]
    zend = detection_zone[1]

    mc = ModuleConnector(device_name, log_level=0)

    x2m200 = mc.get_x2m200()
    x2m200.load_sleep_profile()
    x2m200.subscribe_to_resp_status('resp_status')

    x2m200.set_detection_zone(zstart, zend)
    x2m200.set_sensor_mode_run()

    rdata = x2m200.get_respiration_data('resp_status')

    xhist = 500 # x axis data points
    fps = 20.0
    x=np.arange(0,xhist/fps,1/fps)
    qdist = deque([rdata.distance]*xhist, maxlen=xhist)

    # dict to configure the different plots
    plotdata = { 'sensor_state' : {
                     'minmax'   : (0-0.1, 5),
                     'isubplot' : 1,
                     'ylabel'   : "Sensor state ",
                     },
                 'respiration_rate' : {
                         'minmax'   : (8,20),
                         'isubplot' : 2,
                         'ylabel'   : "Respiration rate\n(RPM)",
                         },
                 'movement' : {
                     'minmax'   : (-5, 5),
                     'isubplot' : 3,
                     'ylabel'   : "Movement\n(mm)",
                     },
                 'distance' : {
                     'minmax'   : (zstart-0.1, zend+0.1),
                     'isubplot' : 4,
                     'ylabel'   : "Distance\n(m)",
                     },

            }

    fig, axs = plt.subplots(nrows = len(plotdata), ncols = 1, figsize = (10,10))
    fig.canvas.mpl_connect('resize_event', lambda x: plt.tight_layout())
    fig.suptitle("X2M200 respiration message example", y=0.995)

    # used by animator, will be filled with all lines to update
    lines=[]

    for key, settings in plotdata.iteritems():
        # make data queue
        qdata = deque([rdata.__getattr__(key)]*xhist, maxlen=xhist)
        # add max/min data for y limits
        maxmin = settings['minmax']
        qdata.appendleft(maxmin[0])
        qdata.appendleft(maxmin[1])

        # plot data
        ax = axs[settings['isubplot']-1]
        settings['line'], = ax.plot(x, qdata)
        settings['data'] = qdata
        lines.append(settings['line'])
        ax.set_title(key)
        ax.set_ylabel(settings['ylabel'])
        ax.grid(1)

    states = ['Breathing',
              'Movement',
              'Movement, tracking',
              'NoMovement',
              'Initializing',
              'Error',
              'Unknown']

    ax = axs[plotdata['sensor_state']['isubplot']-1]
    locs = ax.set_yticks(range(len(states)))
    labels = ax.set_yticklabels(states)

    def animate(i):
        rdata = x2m200.get_respiration_data('resp_status')
        for key, settings in plotdata.iteritems():
            settings['data'].append(rdata.__getattr__(key))
            settings['line'].set_ydata(settings['data'])  # update the data
        return lines


    def init():
        for line in lines:
            line.set_ydata(np.ma.array(x, mask=True))

        return lines

    # Clear data in the pipe
    x2m200.clear('resp_status')

    ani = animation.FuncAnimation(fig, animate, frames=None, init_func=init,
                                  interval=1000./(2*fps), blit=True)

    plt.tight_layout()

    plt.show()
    return x2m200


def main():
    import sys
    from optparse import OptionParser
    parser = OptionParser()
    parser.add_option(
        "-d",
        "--device",
        dest="device_name",
        help="device file to use, example: python %s -d COM4"%sys.argv[0],
        metavar="FILE")

    parser.add_option('-z', '--detection_zone', nargs=2, type='float',
        help='Start and stop of detection zone.', metavar='START STOP',
        default=(0.4, 1))

    (options, args) = parser.parse_args()

    if not options.device_name:
        print("Please specify a device name, example: python %s -d COM4"%sys.argv[0])
        sys.exit(1)
    x2m200_respiration_message_example(**vars(options))


if __name__ == "__main__":
    main()
