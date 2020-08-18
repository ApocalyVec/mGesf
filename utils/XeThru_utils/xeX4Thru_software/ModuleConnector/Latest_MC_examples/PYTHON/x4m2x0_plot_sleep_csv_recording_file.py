#!/usr/bin/env python
""" \example x4m2x0_plot_sleep_csv_recording_file.py

Latest examples is located at https://github.com/xethru/XeThru_ModuleConnector_Examples or https://dev.azure.com/xethru/XeThruApps/_git/XeThru_ModuleConnector_Examples.

# Target:
# X4M200/X4M210 sleep recording file.


# Introduction:
# X4M200/X4M210 support sleep message output, which contain respiration and hear rate(only X4M210 support) information. The message can be record as sleep recording file, for example xethru_sleep_20190124_141808.csv.
# This script can plot sleep recording file to show the data for one periode, for example one night.

# Command to run:
$ python plot_vitalsigns_csv_recording_file.py --savefig --report xethru_sleep_xxx_xxx.csv
This generates a matplotlibplot which allows zooming and storing the plot as an image. The --savefig option stores the plot as a png image in the same folder as the csv file.
"""
from __future__ import division, print_function
from matplotlib import pyplot as plt
import matplotlib.dates as mdate
from numpy import loadtxt
import numpy as np


def get_log_header_nrow(fname, delimiter=';'):
    """Expects data to start after the first line starting with a letter
    """
    from string import ascii_letters
    startrow = 0
    comments = ''
    with open(fname, 'r') as f:
        while 1:
            line = f.readline().rstrip()
            if line == '':
                startrow = -1
                break
            startrow += 1
            if line[0] in ascii_letters:
                break
            comments += line+'\n'

    return startrow, line.split(delimiter), comments


def read_log(fname):
    """ Reads a XeThru Respiration log file

    Returns: dict with the log file values
    """
    import dateutil
    from matplotlib.dates import date2num
    from collections import OrderedDict

    delimiter = ";"
    startrow, header, comments = get_log_header_nrow(fname, delimiter)

    def datestr2num(x): return date2num(
        dateutil.parser.parse(x, ignoretz=True))

    data = loadtxt(fname, delimiter=delimiter,
                   skiprows=startrow, converters={0: datestr2num})

    res = OrderedDict()

    for ifield, name in enumerate(header):
        res[name] = data[:, ifield]
    res['comments'] = comments

    return res


def get_stateness(states, statearray):
    res = np.zeros(len(states))

    for i in range(len(res)):
        res[i] = sum(statearray == i)/len(statearray)

    return res


def report(logfile, savefig=False):
    """!Read and plot a XeThru log file.

    @param logfile: path to csv file
    @param savefig: bool, saves a png image of the plotted result

    @return: Figure object
    """

    states = ['Breathing',
              'Movement',
              'Movement, tracking',
              'NoMovement',
              'Initializing',
              'Error',
              'Unknown']

    # The following data is only valid in Breathing state:
    breathing_state_data = ['ObjectDistance',
                            'ObjectMovement', 'SignalQuality']

    sens = read_log(logfile)
    # Remove data not to be plotted
    timestamp = sens.pop('TimeStamp')
    framecounter = sens.pop('FrameCounter', None)
    comments = sens.pop('comments', None)

    # Number of data sets to plot
    M = len(sens.keys())

    # Summarize time in each state
    stateness = get_stateness(states, sens['State'])
    sstateness = ''
    for i in range(len(stateness)):
        sstateness += "%s: %4.2f %% \n" % (states[i], stateness[i]*100)

    fig, axs = plt.subplots(M, 1, sharex=True, figsize=(20, 12))
    fig.suptitle(logfile)

    for ikey, key in enumerate(sens.keys()):
        ax = axs[ikey]
        ax.set_title(key)

        # Mask invalid data
        if key in breathing_state_data:
            data = np.ma.masked_where(sens['State'] > 2, sens[key])
        else:
            data = sens[key]
        if key == 'RPM':
            data = np.ma.masked_where(sens['State'] != 0, sens[key])

        ax.plot_date(timestamp, data, color='#4B0082', fmt='-')

        # Data specific plotting rules
        if key == 'State':
            locs = ax.set_yticks(range(len(states)))
            labels = ax.set_yticklabels(states)
            ax.text(0.9, 0, sstateness, transform=ax.transAxes)

        if key == 'SignalQuality':
            ax.set_ylabel("Signal Quality (0-10)")
            ax.set_ylim(-0.1, 10.9)

        ax.grid()
        # ax.set_ylabel(key)

    ax.set_xlabel("Time")

    # xtick format string
    date_fmt = '%H:%M:%S'

    # Use a DateFormatter to set the data to the correct format.
    date_formatter = mdate.DateFormatter(date_fmt)
    ax.xaxis.set_major_formatter(date_formatter)

    fig.autofmt_xdate()

    plt.tight_layout()
    plt.subplots_adjust(top=0.92)

    if savefig:
        fig.savefig(logfile+'.png')

    return fig


def report_all(folder, savefig=False):
    from glob import glob

    logfiles = glob(folder+'/*.csv')

    for logfile in logfiles:
        report(logfile, savefig=savefig)


def main():
    import argparse

    parser = argparse.ArgumentParser(
        description='XeThru Respiration log plotter')
    parser.add_argument('--report', type=str,
                        help="Report measurement")
    parser.add_argument('--report-all', type=str,
                        help="Report all measurements")
    parser.add_argument('--savefig', action="store_true",
                        help="Save the figure")

    args = parser.parse_args()

    if args.report:
        report(args.report, savefig=args.savefig)
        plt.show()
    elif args.report_all:
        report_all(args.report_all, savefig=args.savefig)
    else:
        parser.parse_args(['-h'])


if __name__ == "__main__":
    from matplotlib import pyplot as plt
    plt.ion()
    main()
    plt.show()
    #raw_input("[enter] to exit")
