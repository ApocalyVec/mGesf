import os
import pickle
import time
from datetime import datetime

from PyQt5 import QtWidgets
from PyQt5.QtCore import QTimer, pyqtSlot
from PyQt5 import QtCore

from PyQt5.QtGui import QPixmap
from PyQt5.QtWidgets import QGraphicsPixmapItem, QWidget, QMainWindow, QLabel, QVBoxLayout, QPushButton, QTabWidget, \
    QGraphicsScene, QGraphicsView
import pyqtgraph as pg

from utils.img_utils import array_to_colormap_qim

import numpy as np
import mGesf.MMW_worker as MMW_worker
from utils.iwr6843_utils.mmWave_interface import MmWaveSensorInterface

import mGesf.exceptions as exceptions

def init_view(label):
    vl = QtWidgets.QVBoxLayout()
    ql = QLabel()
    ql.setAlignment(QtCore.Qt.AlignCenter)
    ql.setText(label)
    vl.addWidget(ql)
    return vl, ql


class Control_tab(QWidget):
    def __init__(self, mmw_worker: MMW_worker, refresh_interval, data_path, *args, **kwargs):
        super().__init__()

        self.mmw_worker = mmw_worker
        # create mmWave layout #####################################################
        main_hl = QtWidgets.QHBoxLayout(self)
        # set the mGesf layout
        self.setLayout(main_hl)

        self.control_vl = QtWidgets.QVBoxLayout()  # create vbox for controls
        self.figure_gl = QtWidgets.QGridLayout()  # create grid layout for the figures

        main_hl.addLayout(self.control_vl)
        main_hl.addLayout(self.figure_gl)

        # connect the mmWave frame signal to the function that processes the data
        self.mmw_worker.signal_mmw_control_tab.connect(self.control_process_mmw_data)

        # add the figures ##################################

        # print("Packet ID:\t%d "%(frameNum))
        # print("Version:\t%x "%(version))
        # print("Data Len:\t\t%d", length)
        # print("TLV:\t\t%d "%(numTLVs))
        # print("Detect Obj:\t%d "%(numObj))
        # print("Platform:\t%X "%(platform))

        # add range doppler
        self.doppler_display = QGraphicsPixmapItem()
        self.init_spec_view(pos=(1, 1), label='Range Doppler Profile')

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

        # create the data buffers
        self.buffer = {'mmw': {'timestamps': [], 'ra_profile': []}}
        self.data_path = data_path

        # prepare the sensor interface
        # if mmw_interface:
        #     print('App: using IWR6843AoP; starting sensor')
        #     self.mmw_worker.start_mmw()
        #     print('App: done!')
        # else:
        #     print('App: not using IWR6843AoP')

        self.show()

    def init_spec_view(self, pos, label):
        vl, ql = init_view(label)

        spc_gv = QGraphicsView()
        vl.addWidget(spc_gv)

        self.figure_gl.addLayout(vl, *pos)
        scene = QGraphicsScene(self)
        spc_gv.setScene(scene)
        scene.addItem(self.doppler_display)
        return scene

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
            self.mmw_worker.record_mmw()
            self.record_btn.setText("Stop Recording")
        else:
            self.is_record = False
            self.mmw_worker.end_record_mmw()
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
        usr_path = self.config_textbox.text()
        if os.path.exists(usr_path):
            self.mmw_worker.send_config(config_path=usr_path)
        else:
            raise exceptions.StoragePathInvalidError

    def control_process_mmw_data(self, data_dict):
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

        # save the data is record is enabled
        # mmw buffer: {'timestamps': [], 'ra_profile': [], 'rd_heatmap': [], 'detected_points': []}
        if self.is_record:
            self.buffer['mmw']['timestamps'].append(time.time())
            self.buffer['mmw']['ra_profile'].append(data_dict['range_doppler'])


    @pyqtSlot()
    def on_click(self):
        print("\n")
        for currentQTableWidgetItem in self.tableWidget.selectedItems():
            print(currentQTableWidgetItem.row(), currentQTableWidgetItem.column(), currentQTableWidgetItem.text())
