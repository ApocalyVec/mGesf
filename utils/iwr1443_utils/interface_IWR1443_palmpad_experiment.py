import _thread
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
from utils.data_utils import preprocess_frame
from utils.iwr1443_utils import readAndParseData14xx, parseConfigFile

isPredict = False

configFileName = '1443config.cfg'

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


# ------------------------------------------------------------------

# Funtion to update the data and display in the plot
def update():
    dataOk = 0
    global detObj

    x = []
    y = []
    z = []
    doppler = []

    # Read and parse the received data
    dataOk, frameNumber, detObj = readAndParseData14xx(Dataport, configParameters)

    if dataOk:
        # print(detObj)
        x = -detObj["x"]
        y = detObj["y"]
        z = detObj["z"]
        doppler = detObj["doppler"]  # doppler values for the detected points in m/s

    s_original.setData(x, y)
    s_processed.setData(z, doppler)

    QtGui.QApplication.processEvents()

    return dataOk


# -------------------------    MAIN   -----------------------------------------
today = datetime.datetime.now()
today = datetime.datetime.now()

root_dn = 'data/f_data-' + str(today).replace(':', '-').replace(' ', '_')

warnings.simplefilter('ignore', np.RankWarning)
# Configurate the serial port
CLIport, Dataport = serialConfig(configFileName)

# Get the configuration parameters from the configuration file
configParameters = parseConfigFile(configFileName)

# START QtAPPfor the plot
app = QtGui.QApplication([])

# Set the plot
pg.setConfigOption('background', 'w')
win = pg.GraphicsWindow(title="2D scatter plot")
p_original = win.addPlot()
p_original.setXRange(-0.5, 0.5)
p_original.setYRange(0, 1.5)
p_original.setLabel('left', text='Y position (m)')
p_original.setLabel('bottom', text='X position (m)')
s_original = p_original.plot([], [], pen=None, symbol='o')

# set the processed plot
p_processed = win.addPlot()
p_processed.setXRange(-1, 1)
p_processed.setYRange(-1, 1)
p_processed.setLabel('left', text='Z position (m)')
p_processed.setLabel('bottom', text='Doppler (m/s)')
s_processed = p_processed.plot([], [], pen=None, symbol='o')

# Main loop
detObj = {}
frameData = {}
preprocessed_frameArray = []
frameArray = []

# reading RNN model

if isPredict:
    # regressive_classifier = NeuralNetwork()
    # regressive_classifier.load(file_name='trained_models/radar_model/072319_02/regressive_classifier.h5')
    onNotOn_ann_classifier = NeuralNetwork()
    onNotOn_ann_classifier.load(file_name='F:/config_detection/models/onNotOn_ANN/classifier_080919_2.h5')
    onNotOn_encoder = pickle.load(open('F:/config_detection/models/onNotOn_ANN/encoder_080919_2', 'rb'))

rnn_timestep = 100
num_padding = 50

def input_thread(a_list):
    input()
    interrupt_list.append(True)


class prediction_thread(Thread):
    def __init__(self, event):
        Thread.__init__(self)
        self.stopped = event

    def run(self):
        while not self.stopped.wait(0.5):
            prediction_funct_onNotOn_ANN()


def prediction_funct_onNotOn_ANN():
    if len(preprocessed_frameArray) > 0:
        p = onNotOn_ann_classifier.predict(np.asarray([preprocessed_frameArray[len(preprocessed_frameArray) - 1]]))
        p = onNotOn_encoder.inverse_transform(p)
        p = p[0][0]
        if p == 1:
            print('No cluster')
        elif p == 2:
            print('Thumb is setting')
        elif p == 3:
            print('Thumb is raising')
        elif p == 4:
            print('Thumb is rubbing')
        elif p == 5:
            print('Thumb is rubbing in air')


# def prediction_func_onNoton_RNN():
#     if (len(frameData)) < rnn_timestep:
#         print('Warming up')
#     else:
#         x = []
#
#         pred_data = list(frameData.items())
#         pred_data = pred_data[len(pred_data) - rnn_timestep:]
#
#         for entry in pred_data:
#             data = entry[1]
#             data = np.asarray([data['x'], data['y'], data['z'], data['doppler']]).transpose()
#             if data.shape[0] > num_padding:
#                 raise Exception('Insufficient Padding')
#             data = np.pad(data, ((0, num_padding - data.shape[0]), (0, 0)), 'constant', constant_values=0)
#             data = data.reshape((400,))  # flatten
#
#             x.append(data)  # add one additional dimension
#
#         x = np.asarray(x)
#         x = np.expand_dims(x, axis=0)
#
#         if x.shape != (1, 100, 400):
#             print('Prediction: BAD Input Shape')
#         else:
#             prediction_result = regressive_classifier.predict(x)
#             # print('Prediction: ' + str(prediction_result[0][0]), end='      ')
#             print('Prediction: ', end='      ')
#
#             if prediction_result[0][0] > 0.5:
#                 print('Thumb is ON Pointing Finger')
#             else:
#                 print('Thumb is NOT ON Pointing Finger')


input("Press Enter to start!...")

# create the interrupt thread
interrupt_list = []
_thread.start_new_thread(input_thread, (interrupt_list,))

# start the prediction thread
main_stop_event = Event()
if isPredict:
    thread = prediction_thread(main_stop_event)
    thread.start()

start_time = time.time()

while True:
    try:
        # Update the data and check if the data is okay
        dataOk = update()

        if dataOk:
            # Store the current frame into frameData

            frameRow = np.asarray([detObj['x'], detObj['y'], detObj['z'], detObj['doppler']]).transpose()
            frameArray.append(time.time(), frameRow)
            preprocessed_frameArray.append(preprocess_frame(frameRow))

        time.sleep(0.033)  # This is framing frequency Sampling frequency of 30 Hz

        if interrupt_list:
            raise KeyboardInterrupt()

    # Stop the program and close everything if Ctrl + c is pressed

    except KeyboardInterrupt:
        CLIport.write(('sensorStop\n').encode())
        CLIport.close()
        Dataport.close()
        win.close()

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
