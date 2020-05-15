import _thread
import collections
import pickle
from threading import Thread, Event

import serial
import numpy as np
import pyqtgraph as pg

from pyqtgraph.Qt import QtGui

import datetime
import os
import time

import warnings

from classes.model_wrapper import NeuralNetwork
from utils.iwr1443_utils import readAndParseData14xx, parseConfigFile

isPredict = False

configFileName = 'D:/PycharmProjects/mmWaveGesture/1443config_new.cfg'
data_q = collections.deque(maxlen=10)

CLIport = {}
Dataport = {}

"""
# global to hold the average x, y, z of detected points

those are for testing swipe gestures
note that for now, only x and y are used

those values is updated in the function: update
"""
x = []
y = []
z = []
doppler = []


# ------------------------------------------------------------------

# Function to configure the serial ports and send the data from
# the configuration file to the radar
def serialConfig(configFileName):
    global CLIport
    global Dataport
    # Open the serial ports for the configuration and the data ports

    # Raspberry pi
    # CLIport = serial.Serial('/dev/ttyACM0', 115200)
    # Dataport = serial.Serial('/dev/ttyACM1', 921600)

    # For WINDOWS, CHANGE those serial port to match your machine's configuration
    CLIport = serial.Serial('COM5', 115200)
    Dataport = serial.Serial('COM4', 921600)

    # Read the configuration file and send it to the board
    config = [line.rstrip('\r\n') for line in open(configFileName)]
    for i in config:
        CLIport.write((i + '\n').encode())
        print(i)
        time.sleep(0.01)

    return CLIport, Dataport


# ------------------------------------------------------------------

# Funtion to update the data and display in the plot
def update():
    # Read and parse the received data
    dataOk, frameNumber, detObj = readAndParseData14xx(Dataport, configParameters)

    return dataOk, frameNumber, detObj


# -------------------------    MAIN   -----------------------------------------
today = datetime.datetime.now()

root_dn = 'data/f_data-' + str(today).replace(':', '-').replace(' ', '_')

warnings.simplefilter('ignore', np.RankWarning)
# Configurate the serial port
CLIport, Dataport = serialConfig(configFileName)

# Get the configuration parameters from the configuration file
configParameters = parseConfigFile(configFileName)

# Main loop
detObj = {}
frameData = []
preprocessed_frameArray = []

# reading RNN model

rnn_timestep = 100
num_padding = 50


def input_thread(a_list):
    input()
    interrupt_list.append(True)


class PredicationThread(Thread):
    def __init__(self, event):
        Thread.__init__(self)
        self.stopped = event

    def run(self):
        while not self.stopped.wait(0.5):
            pass


input("Press Enter to start!...")
print('Started!')

# create the interrupt thread
interrupt_list = []
_thread.start_new_thread(input_thread, (interrupt_list,))

# start the prediction thread
main_stop_event = Event()
if isPredict:
    thread = PredicationThread(main_stop_event)
    thread.start()

start_time = time.time()

while True:
    try:
        # Update the data and check if the data is okay
        dataOk, frameNumber, detObj = update()

        if dataOk:
            # Store the current frame into frameData
            frameData.append((time.time(), detObj))
            data_q.append(detObj)

            # frameRow = np.asarray([detObj['x'], detObj['y'], detObj['z'], detObj['doppler']]).transpose()
            # preprocessed_frameArray.append(preprocess_frame(frameRow))

            # time.sleep(0.033)  # This is framing frequency Sampling frequency of 30 Hz

        if interrupt_list:
            raise KeyboardInterrupt()

    # Stop the program and close everything if Ctrl + c is pressed

    except KeyboardInterrupt:
        CLIport.write(('sensorStop\n').encode())
        CLIport.close()
        Dataport.close()

        # stop prediction thread
        main_stop_event.set()

        # save radar frame data

        is_save = input('do you wish to save the recorded frames? [y/n]')

        if is_save == 'y':
            os.mkdir(root_dn)
            file_path = os.path.join(root_dn, 'f_data.p')
            with open(file_path, 'wb') as pickle_file:
                pickle.dump(frameData, pickle_file)
        else:
            print('exit without saving')

        break
