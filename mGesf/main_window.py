import os
import pickle
import time
from datetime import datetime

from PyQt5 import QtWidgets
from PyQt5.QtCore import QTimer, pyqtSlot
from PyQt5.QtGui import QPixmap
from PyQt5.QtWidgets import QGraphicsPixmapItem, QWidget, QMainWindow, QLabel, QVBoxLayout, QPushButton, QTabWidget
import pyqtgraph as pg

from utils.iwr6843_utils.mmWave_interface import MmWaveSensorInterface
from utils.img_utils import array_to_colormap_qim

import numpy as np
import mGesf.view as view
import mGesf.MMW_worker as MMW_worker

# class WorkerSignals(QObject):
#     finished = pyqtSignal()
#     error = pyqtSignal(tuple)
#     result = pyqtSignal(object)
#     progress = pyqtSignal(int)

WINDOW_HEIGHT = 720
WINDOW_LENGTH = 1280


# TODO add resume function to the stop button
class MainWindow(QMainWindow):
    def __init__(self, mmw_interface: MmWaveSensorInterface, refresh_interval, data_path, *args, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)
        self.resize(WINDOW_LENGTH, WINDOW_HEIGHT)
        pg.setConfigOption('background', 'w')
        self.mmw_interface = mmw_interface

        # create the frame
        self.table_widget = Frame(self, mmw_interface=mmw_interface, refresh_interval=refresh_interval,
                                  data_path=data_path)
        self.setCentralWidget(self.table_widget)
        self.show()


class Frame(QWidget):
    """A frame contains 4 tabs and their contents"""

    def __init__(self, parent, mmw_interface: MmWaveSensorInterface, refresh_interval, data_path, *args, **kwargs):
        super(QWidget, self).__init__(parent)
        self.layout = QVBoxLayout(self)

        # Initialize tab screen
        self.tabs = QTabWidget()
        self.tab1 = QWidget()
        self.tab1.resize(200,100)
        self.tab2 = QWidget()
        self.tab3 = QWidget()
        self.tab4 = QWidget()

        self.tabs.addTab(self.tab1, "CONTROL")
        self.tabs.addTab(self.tab2, "RADAR")
        self.tabs.addTab(self.tab3, "LEAP")
        self.tabs.addTab(self.tab4, "UWB")

        self.tab1.layout = QVBoxLayout(self)
        self.tab2.layout = QVBoxLayout(self)
        self.tab3.layout = QVBoxLayout(self)
        self.tab4.layout = QVBoxLayout(self)

        # Add tabs to widget
        self.layout.addWidget(self.tabs)
        self.setLayout(self.layout)

        # create mmWave layout #####################################################
        main_hl = QtWidgets.QHBoxLayout(self)
        self.control_vl = QtWidgets.QVBoxLayout()  # create vbox for controls
        self.figure_gl = QtWidgets.QGridLayout()  # create grid layout for the figures

        main_hl.addLayout(self.control_vl)
        main_hl.addLayout(self.figure_gl)

        # create threading
        # create a QThread and start the thread that handles
        self.worker_thread = pg.QtCore.QThread(self)
        self.worker_thread.start()

        # timer
        self.timer = QTimer()
        self.timer.setInterval(refresh_interval)
        self.timer.timeout.connect(self.ticks)

        # worker
        self.mmw_worker = MMW_worker.MmwWorker(mmw_interface)
        self.mmw_worker.moveToThread(self.worker_thread)
        # connect the mmWave frame signal to the function that processes the data
        self.mmw_worker.signal_mmw_frame_ready.connect(self.process_mmw_data)

        # add the figures ##################################
        # add statistics
        self.statistics_ui, self.statistics_vl = view.init_statistics(self.figure_gl)

        # print("Packet ID:\t%d "%(frameNum))
        # print("Version:\t%x "%(version))
        # print("Data Len:\t\t%d", length)
        # print("TLV:\t\t%d "%(numTLVs))
        # print("Detect Obj:\t%d "%(numObj))
        # print("Platform:\t%X "%(platform))

        # add range doppler
        self.doppler_display = QGraphicsPixmapItem()
        view.init_spec(self, pos=(1, 1), label='Range Doppler Profile')

        # add range azi
        self.azi_display = QGraphicsPixmapItem()
        view.init_spec(self, pos=(1, 2), label='Range Azimuth Profile')

        # add detected points plots
        self.scatterXY = view.init_pts_view(self, pos=(0, 1), label='Detected Points XY', x_lim=(-0.5, 0.5),
                                            y_lim=(0, 1.))
        self.scatterZD = view.init_pts_view(self, pos=(0, 2), label='Detected Points ZD', x_lim=(-0.5, 0.5),
                                            y_lim=(-1., 1.))
        self.ra_view = view.init_curve(self, pos=(1, 0), label='Range Profile', x_lim=(-10, 260),
                                       y_lim=(1500, 3800))

        # add the controls ##################################

        # add the interrupt button
        self.start_stop_btn = QtWidgets.QPushButton(text='Start Sensor')
        self.start_stop_btn.clicked.connect(self.start_stop_btn_action)
        self.control_vl.addWidget(self.start_stop_btn)

        # add the start record button
        self.is_record = False
        self.record_btn = QtWidgets.QPushButton(text='Start Recording')
        self.record_btn.clicked.connect(self.record_btn_action)
        self.control_vl.addWidget(self.record_btn)

        # com port entries
        # Create textbox
        self.uport_textbox = QtWidgets.QLineEdit()
        self.uport_textbox.setPlaceholderText('User Port')
        self.control_vl.addWidget(self.uport_textbox)

        # Create textbox
        self.dport_textbox = QtWidgets.QLineEdit()
        self.dport_textbox.setPlaceholderText('Data Port')
        self.control_vl.addWidget(self.dport_textbox)

        # add close connection button
        self.connection_btn = QtWidgets.QPushButton(text='Connect')
        self.connection_btn.clicked.connect(self.connection_btn_action)
        self.control_vl.addWidget(self.connection_btn)

        # Create textbox
        self.config_textbox = QtWidgets.QLineEdit()
        self.config_textbox.setPlaceholderText('Config File Path')
        self.control_vl.addWidget(self.config_textbox)

        # add send config button
        self.config_btn = QtWidgets.QPushButton(text='Send Config')
        self.config_btn.clicked.connect(self.config_btn_action)
        self.config_btn.clicked.connect(self.config_btn_action)
        self.control_vl.addWidget(self.config_btn)

        # add label (running or stopped.)
        self.dialogueLabel = QLabel()
        self.dialogueLabel.setText("Running")
        self.control_vl.addWidget(self.dialogueLabel)

        # set the mGesf layout
        self.tab1.setLayout(main_hl)

        # create the data buffers
        self.buffer = {'mmw': {'timestamps': [], 'ra_profile': [], 'rd_heatmap': [], 'detected_points': []}}
        self.data_path = data_path

        # prepare the sensor interface
        # if mmw_interface:
        #     print('App: using IWR6843AoP; starting sensor')
        #     self.mmw_worker.start_mmw()
        #     print('App: done!')
        # else:
        #     print('App: not using IWR6843AoP')

        self.timer.start()

    def config_btn_action(self):
        # TODO add check if file exits
        self.mmw_worker.send_config(self.config_textbox.text())

    def start_stop_btn_action(self):
        if self.mmw_worker.is_mmw_running():
            self.start_stop_btn.setText('Start Sensor')
            self.mmw_worker.stop_mmw()
            self.dialogueLabel.setText('Stopped.')
        else:
            self.start_stop_btn.setText('Stop Sensor')
            self.mmw_worker.start_mmw()
            self.dialogueLabel.setText('Running.')

    def record_btn_action(self):
        if not self.is_record:
            self.is_record = True
            self.record_btn.setText("Stop Recording")
        else:
            self.is_record = False
            self.record_btn.setText("Start Recording")

            today = datetime.now()
            pickle.dump(self.buffer, open(os.path.join(self.data_path,
                                                       today.strftime("%b-%d-%Y-%H-%M-%S") + '.mgesf'), 'wb'))
            print('data save to ' + self.data_path)

    def connection_btn_action(self):
        if self.mmw_worker.is_connected():
            self.mmw_worker.disconnect_mmw()
            self.connection_btn.setText('Connect')
        else:
            self.mmw_worker.connect_mmw(uport_name=self.uport_textbox.text(), dport_name=self.dport_textbox.text())
            self.connection_btn.setText('Disconnect')

    def config_btn_action(self):
        self.mmw_worker.send_config(config_path=self.config_textbox.text())

    def process_mmw_data(self, data_dict):
        """
        Process the emitted mmWave data
        This function is evoked when signaled by self.mmw_data_ready which is emitted by the mmw_worker thread.
        The function handles the following actions
            update the mmw figures in the GUI
            record the mmw data if record is enabled. In the current implementation, the data is provisionally saved in
            the memory and evicted when the user click 'stop_record'
        :param data_dict:
        """
        # update range doppler spectrogram
        doppler_heatmap_qim = array_to_colormap_qim(data_dict['range_doppler'])
        doppler_qpixmap = QPixmap(doppler_heatmap_qim)
        doppler_qpixmap = doppler_qpixmap.scaled(512, 512, pg.QtCore.Qt.KeepAspectRatio)  # resize spectrogram
        self.doppler_display.setPixmap(doppler_qpixmap)

        # update range azimuth spectrogram
        azi_heatmap_qim = array_to_colormap_qim(data_dict['range_azi'])
        azi_qpixmap = QPixmap(azi_heatmap_qim)
        azi_qpixmap = azi_qpixmap.scaled(512, 512, pg.QtCore.Qt.KeepAspectRatio)  # resize spectrogram
        self.azi_display.setPixmap(azi_qpixmap)

        # update the 2d scatter plot for the detected points
        self.scatterXY.setData(data_dict['pts'][:, 0], data_dict['pts'][:, 1])
        self.scatterZD.setData(data_dict['pts'][:, 2], data_dict['pts'][:, 3])

        # update range amplitude profile
        ra = np.asarray(data_dict['range_amplitude'])
        range_bin_space = np.asarray(range(len(ra)))
        self.ra_view.setData(range_bin_space, ra)

        # save the data is record is enabled
        # mmw buffer: {'timestamps': [], 'ra_profile': [], 'rd_heatmap': [], 'detected_points': []}
        if self.is_record:
            self.buffer['mmw']['timestamps'].append(time.time())
            self.buffer['mmw']['ra_profile'].append(data_dict['range_doppler'])
            self.buffer['mmw']['rd_heatmap'].append(ra)
            self.buffer['mmw']['detected_points'].append(data_dict['pts'])

    @pg.QtCore.pyqtSlot()
    def ticks(self):
        """
        ticks every 'refresh' milliseconds
        """
        self.mmw_worker.tick_signal.emit()  # signals the worker to run process_on_tick

    @pyqtSlot()
    def on_click(self):
        print("\n")
        for currentQTableWidgetItem in self.tableWidget.selectedItems():
            print(currentQTableWidgetItem.row(), currentQTableWidgetItem.column(), currentQTableWidgetItem.text())