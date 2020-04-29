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


def init_view(label, center=True):
    vl = QtWidgets.QVBoxLayout()
    ql = QLabel()
    if center:
        ql.setAlignment(QtCore.Qt.AlignCenter)
    ql.setText(label)
    vl.addWidget(ql)
    return vl


class Control_tab(QWidget):

    def __init__(self, mmw_worker: MMW_worker, refresh_interval, data_path, *args, **kwargs):
        super().__init__()

        # mmW worker #####################################################
        self.mmw_worker = mmw_worker
        # connect the mmWave frame signal to the function that processes the data
        self.mmw_worker.signal_mmw_control_tab.connect(self.control_process_mmw_data)

        # create the data buffers
        self.buffer = {'mmw': {'timestamps': [], 'ra_profile': []}}
        self.data_path = data_path

        self.is_record = False

        # create mmWave layout #####################################################
        main_page = QtWidgets.QHBoxLayout(self)
        # set the mGesf layout
        self.setLayout(main_page)

        self.radar_thumbnail = QtWidgets.QVBoxLayout()  # create vbox for controls
        self.leap_uwb_record = QtWidgets.QVBoxLayout()  # create vbox for controls
        # self.figure_gl = QtWidgets.QGridLayout()  # create grid layout for the figures

        main_page.addLayout(self.radar_thumbnail)
        main_page.addLayout(self.leap_uwb_record)

        # add range doppler
        self.doppler_display = QGraphicsPixmapItem()

        #  ######################### add sections in radar  ################################

        self.radar_thumbnail_vl = self.init_container(parent=self.radar_thumbnail, label="Radar")
        self.radar_connection_panel = self.init_container(parent=self.radar_thumbnail_vl,  label="Connection")
        self.radar_connection_panel.setContentsMargins(10,10,10,10)
        self.radar_sensor_panel = self.init_container(parent=self.radar_thumbnail_vl,  label="Sensor")
        self.radar_runtime_graph = self.init_spec_view(parent=self.radar_thumbnail_vl, label="Runtime")

        self.leap_uwb_record_thumbnail_vl = self.init_container(parent=self.leap_uwb_record, label="????Test")
        self.leap_runtime_graph = self.init_spec_view(parent=self.leap_uwb_record_thumbnail_vl, label="runtime")

        # ############################# Connection panel ##################################
        # com port entries
        self.d_port_vl = self.init_container(parent=self.radar_connection_panel, label='Data Port (Standard)', center_label=False)
        self.dport_textbox = QtWidgets.QLineEdit()
        self.dport_textbox.setPlaceholderText('default: COM3')
        self.radar_connection_panel.addWidget(self.dport_textbox)

        self.d_port_vl = self.init_container(parent=self.radar_connection_panel, label='User Port (Enhanced)', center_label=False)
        self.uport_textbox = QtWidgets.QLineEdit()
        self.uport_textbox.setPlaceholderText('default: COM14')
        self.radar_connection_panel.addWidget(self.uport_textbox)

        # add close connection button
        self.radar_connection_btn = QtWidgets.QPushButton(text='Connect')
        self.radar_connection_btn.clicked.connect(self.radar_connection_btn_action)
        self.radar_connection_panel.addWidget(self.radar_connection_btn)

        # add U/D port feedback (Connected/invalid U/Dport)
        self.UD_connection_feedback = QLabel()
        self.UD_connection_feedback.setText("Disconnected")
        self.radar_connection_panel.addWidget(self.UD_connection_feedback)

        # ############################# Sensor panel ##################################

        self.is_valid_config_path = False

        # Config path
        self.config_textbox = QtWidgets.QLineEdit()
        self.config_textbox.setPlaceholderText('Config File Path')
        self.radar_thumbnail.addWidget(self.config_textbox)

        # add send config button
        self.config_connection_btn = QtWidgets.QPushButton(text='Send Config')
        self.config_connection_btn.clicked.connect(self.radar_config_btn_action)
        self.config_connection_btn.clicked.connect(self.radar_config_btn_action)
        self.radar_thumbnail.addWidget(self.config_connection_btn)

        # add config connection feedback (invalid path/setting done.)
        self.config_connection_feedback = QLabel()
        self.config_connection_feedback.setText("Invalid config path")
        self.radar_connection_panel.addWidget(self.config_connection_feedback)

        # add the interrupt button
        self.sensor_start_stop_btn = QtWidgets.QPushButton(text='Start Sensor')
        self.sensor_start_stop_btn.clicked.connect(self.start_stop_sensor_action)
        self.radar_sensor_panel.addWidget(self.sensor_start_stop_btn)

        '''
        self.is_record = False
        # add the start record button
        self.record_btn = QtWidgets.QPushButton(text='Start Recording')
        self.record_btn.clicked.connect(self.record_btn_action)
        self.radar_thumbnail.addWidget(self.record_btn)
        '''

        # prepare the sensor interface
        # if mmw_interface:
        #     print('App: using IWR6843AoP; starting sensor')
        #     self.mmw_worker.start_mmw()
        #     print('App: done!')
        # else:
        #     print('App: not using IWR6843AoP')

        self.show()

    def init_container(self, parent, label, center_label=True):
        vl = init_view(label,center_label)
        parent.addLayout(vl)
        return vl

    def init_spec_view(self, parent, label):
        vl = init_view(label)

        spc_gv = QGraphicsView()
        vl.addWidget(spc_gv)

        parent.addLayout(vl)
        scene = QGraphicsScene(self)
        spc_gv.setScene(scene)
        scene.addItem(self.doppler_display)
        return scene

    '''
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
    '''

    def radar_connection_btn_action(self):
        if self.mmw_worker.is_connected():
            self.mmw_worker.disconnect_mmw()
            self.dport_textbox.setPlaceholderText('Data Port')
            self.dport_textbox.setPlaceholderText('User Port')
            self.UD_connection_feedback.setText("Disconnected")
            self.radar_connection_btn.setText('Connect')
        else:
            # TODO: CHECK VALID PORTS
            self.mmw_worker.connect_mmw(uport_name=self.uport_textbox.text(), dport_name=self.dport_textbox.text())
            self.UD_connection_feedback.setText("Connected")
            self.radar_connection_btn.setText('Disconnect')

    def radar_config_btn_action(self):
        usr_path = self.config_textbox.text()
        if os.path.exists(usr_path):
            self.is_valid_config_path = True
            self.config_connection_feedback.setText("Setting done")
            self.mmw_worker.send_config(config_path=usr_path)
        else:
            self.is_valid_config_path = False
            self.config_connection_feedback.setText("Invalid path")

    def start_stop_sensor_action(self):
        # TODO: CONNECT WHEN CONFIG PATH VALID
        if self.mmw_worker.is_mmw_running():
            self.sensor_start_stop_btn.setText('Start Sensor')
            self.mmw_worker.stop_mmw()
            self.config_connection_feedback.setText('Stopped.')
        else:
            self.sensor_start_stop_btn.setText('Stop Sensor')
            self.mmw_worker.start_mmw()
            self.config_connection_feedback.setText('Running.')

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
