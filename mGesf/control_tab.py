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
import mGesf.config as config


def init_view(label, position="center", vertical=True):
    if vertical:
        vl = QtWidgets.QVBoxLayout()
    else:
        vl = QtWidgets.QHBoxLayout()
    if label:
        ql = QLabel()
        if position == "center":
            ql.setAlignment(QtCore.Qt.AlignTop)
            ql.setAlignment(QtCore.Qt.AlignCenter)
        elif position == "rightbottom":
            ql.setAlignment(QtCore.Qt.AlignRight)
            ql.setAlignment(QtCore.Qt.AlignBottom)

        ql.setText(label)
        vl.addWidget(ql)
    return vl


def init_container(parent, label=None, label_position=None, vertical=True):
    vl = init_view(label, label_position, vertical)
    parent.addLayout(vl)
    return vl


def setup_radar_connection_button(parent, function):
    btn = QtWidgets.QPushButton(text='Connect')
    btn.clicked.connect(function)
    parent.addWidget(btn)
    return btn


def setup_user_port(parent):
    user_port_block = init_container(parent=parent, label='User Port (Enhanced)',
                                     vertical=False)
    uport_textbox = QtWidgets.QLineEdit()
    uport_textbox.setContentsMargins(5, 0, 0, 0)
    uport_textbox.setPlaceholderText("default: " + config.u_port_default)
    user_port_block.addWidget(uport_textbox)
    return user_port_block, uport_textbox


def setup_data_port(parent):
    data_port_block = init_container(parent=parent, label='Data Port (Standard)',
                                     vertical=False)
    dport_textbox = QtWidgets.QLineEdit()
    dport_textbox.setContentsMargins(10, 0, 0, 0)
    dport_textbox.setPlaceholderText("default: " + config.d_port_default)
    data_port_block.addWidget(dport_textbox)
    return data_port_block, dport_textbox


def setup_datapath_block(parent):
    data_path_block = init_container(parent=parent, label='Output path: ', vertical=False)
    data_path_textbox = QtWidgets.QLineEdit()
    data_path_textbox.setContentsMargins(10, 0, 0, 0)
    data_path_textbox.setPlaceholderText("default: " + config.data_path)
    data_path_block.addWidget(data_path_textbox)

    return data_path_block, data_path_textbox


def setup_information_block(parent):
    information_block = init_container(parent=parent, label="Information")
    message = QLabel()
    message.setText("Hi there")
    information_block.addWidget(message)

    return information_block, message


def setup_record_button(parent, function):
    is_recording = False
    record_btn = QtWidgets.QPushButton(text='Start Recording')
    record_btn.clicked.connect(function)
    parent.addWidget(record_btn)

    return record_btn, is_recording


def setup_config_path_block(parent):
    is_valid_config_path = False
    config_textbox = QtWidgets.QLineEdit()
    config_textbox.setPlaceholderText('default ' + config.config_file_path_default)
    parent.addWidget(config_textbox)

    return is_valid_config_path, config_textbox


def setup_config_btn(parent, function):
    config_connection_btn = QtWidgets.QPushButton(text='Send Config')
    config_connection_btn.clicked.connect(function)
    parent.addWidget(config_connection_btn)

    return config_connection_btn


def setup_sensor_btn(parent, function):
    sensor_start_stop_btn = QtWidgets.QPushButton(text='Start Sensor')
    sensor_start_stop_btn.clicked.connect(function)
    parent.addWidget(sensor_start_stop_btn)

    return sensor_start_stop_btn


class Control_tab(QWidget):

    def __init__(self, mmw_worker: MMW_worker, refresh_interval, *args, **kwargs):
        super().__init__()

        # mmW worker
        self.mmw_worker = mmw_worker
        # connect the mmWave frame signal to the function that processes the data
        self.mmw_worker.signal_mmw_control_tab.connect(self.control_process_mmw_data)
        # create the data buffers
        self.buffer = {'mmw': {'timestamps': [], 'ra_profile': []}}
        # add range doppler
        self.doppler_display = QGraphicsPixmapItem()

        # #################### create mmWave layout #################################

        # -------------------- First class --------------------
        # The whole window
        self.main_page = QtWidgets.QHBoxLayout(self)
        self.setLayout(self.main_page)

        # -------------------- Second class --------------------
        #   1. control block
        #   2. information block

        self.control_block = init_container(parent=self.main_page)
        # ***** information block *****
        self.information_block, self.message = setup_information_block(parent=self.main_page)

        # -------------------- third class --------------------
        #   1. Control block
        #       1-1. RLU block
        #       1-2. Record block

        self.RLU_block = init_container(parent=self.control_block, vertical=False)
        self.record_block = init_container(parent=self.control_block, label_position="rightbottom",
                                           label="Record")

        # -------------------- fourth class -------------------
        #       1-1. RLU block
        #           1-1-1. Radar block
        #           1-1-2. Leap block
        #           1-1-3. UWB block
        self.radar_block = init_container(parent=self.RLU_block, label="Radar", label_position="center")
        self.leap_block = init_container(parent=self.RLU_block, label="LeapMotion", label_position="center")
        self.uwb_block = init_container(parent=self.RLU_block, label="Ultra-Wide-Band",
                                        label_position="center")

        # -------------------- fourth class --------------------
        #       1-2. Record block
        #           1-2-1. Checkmarks
        #           1-2-2. Output path text box & record button
        checkmark_block = init_container(parent=self.record_block)
        # ***** data_path block *****
        self.data_path_block, self.data_path_textbox = setup_datapath_block(parent=self.record_block)
        # ***** record button *****
        self.record_btn, self.is_recording = setup_record_button(parent=self.record_block,
                                                                 function=self.record_btn_action)

        # -------------------- fifth class --------------------
        #           1-1-1. Radar block
        #               1-1-1-1. Connection block
        #               1-1-1-2. Sensor block
        #               1-1-1-3. Runtime block
        self.radar_connection_block = init_container(parent=self.radar_block, label="Connection")
        self.radar_sensor_block = init_container(parent=self.radar_block, label="Sensor")
        self.radar_runtime_block = self.init_spec_view(parent=self.radar_block, label="Runtime")

        # -------------------- sixth class --------------------
        #               1-1-1-1. Connection block
        #                   1-1-1-1-1. Data port block
        #                   1-1-1-1-2. User port block
        #                   1-1-1-1-3. Connect button
        # ***** ports *****
        self.data_port_block, self.dport_textbox = setup_data_port(parent=self.radar_connection_block)
        self.user_port_block, self.uport_textbox = setup_user_port(parent=self.radar_connection_block)
        # ***** connect button *****
        self.radar_connection_btn = setup_radar_connection_button(parent=self.radar_connection_block,
                                                                  function=self.radar_connection_btn_action)

        # -------------------- sixth class --------------------
        #               1-1-1-2. Sensor block
        #                   1-1-1-2-1. Config path block
        #                   1-1-1-2-2. senor buttons block
        #                       1. Send_config Button
        #                       2. Start/Stop sensor button
        self.is_valid_config_path, self.config_textbox = setup_config_path_block(parent=self.radar_sensor_block)
        self.sensor_buttons_block = init_container(self.radar_sensor_block, vertical=False)

        # -------------------- sixth class --------------------
        #               1-1-1-3. Runtime block
        self.radar_runtime_view = self.init_spec_view(parent=self.radar_runtime_block, label="Runtime")

        # -------------------- seventh class --------------------
        #                   1-1-1-2-2. senor buttons block
        #                       1-1-1-2-2-1. Send_config Button
        #                       1-1-1-2-2-2. Start/Stop sensor button
        self.config_connection_btn = setup_config_btn(parent=self.sensor_buttons_block,
                                                      function=self.send_config_btn_action)
        self.sensor_start_stop_btn = setup_sensor_btn(parent=self.sensor_buttons_block,
                                                      function=self.start_stop_sensor_action)



        self.show()

    def init_spec_view(self, parent, label):
        spc_gv = QGraphicsView()
        parent.addWidget(spc_gv)
        scene = QGraphicsScene(self)
        spc_gv.setScene(scene)
        scene.addItem(self.doppler_display)
        return scene

    def record_btn_action(self):
        """ 1. Checks user input data path
            2. use default path in no input
            3. record if path valid
        """
        data_path = self.data_path_textbox.text()
        if not data_path:
            data_path = config.data_path

        if os.path.exists(data_path):
            self.message.setText(config.datapath_set_message + "\nCurrent data path: " + data_path)
            if not self.is_recording:
                self.is_recording = True
                self.mmw_worker.record_mmw()
                self.record_btn.setText("Stop Recording")
            else:
                self.is_recording = False
                self.mmw_worker.end_record_mmw()
                self.record_btn.setText("Start Recording")

                today = datetime.now()
                pickle.dump(self.buffer, open(os.path.join(config.data_path,
                                                           today.strftime("%b-%d-%Y-%H-%M-%S") + '.mgesf'), 'wb'))
                print('Data save to ' + config.data_path)
        else:
            self.message.setText(config.datapath_invalid_message + "\nCurrent data path: " + data_path)

    def radar_connection_btn_action(self):
        """ 1. Get user entered ports
            2. use default ports in no input
            3. Connect if ports valid
        """
        if self.mmw_worker.is_connected():
            self.mmw_worker.disconnect_mmw()
            self.dport_textbox.setPlaceholderText('default ' + config.d_port_default)
            self.dport_textbox.setPlaceholderText('default ' + config.u_port_default)
            self.message.setText(config.UDport_disconnected_message)
            self.radar_connection_btn.setText('Connect')
        else:
            # TODO: CHECK VALID PORTS
            self.mmw_worker.connect_mmw(uport_name=self.uport_textbox.text(), dport_name=self.dport_textbox.text())
            self.message.setText(config.UDport_connected_message)
            self.radar_connection_btn.setText('Disconnect')

    def send_config_btn_action(self):
        """ 1. Get user entered config path
            2. use default config path in no input
            3. Send config if valid
        """

        config_path = self.config_textbox.text()
        if not config_path:
            config_path = config.config_file_path_default

        if os.path.exists(config_path):
            self.is_valid_config_path = True
            self.message.setText(config.config_set_message + "\nCurrent path: " + config_path)
            self.mmw_worker.send_config(config_path=config_path)
        else:
            self.is_valid_config_path = False
            self.message.setText(config.config_invalid_message + "\nCurrent path: " + config_path)

    def start_stop_sensor_action(self):
        # TODO: CONNECT WHEN CONFIG PATH VALID
        if self.mmw_worker.is_mmw_running():
            self.sensor_start_stop_btn.setText('Start Sensor')
            self.mmw_worker.stop_mmw()
            self.message.setText(config.stop_sensor_message)
        else:
            self.sensor_start_stop_btn.setText('Stop Sensor')
            self.mmw_worker.start_mmw()
            self.message.setText(config.start_sensor_message)

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
        if self.is_recording:
            self.buffer['mmw']['timestamps'].append(time.time())
            self.buffer['mmw']['ra_profile'].append(data_dict['range_doppler'])
