import os
import pickle
import time
from datetime import datetime

from PyQt5 import QtWidgets
from PyQt5 import QtCore

from PyQt5.QtGui import QPixmap
from PyQt5.QtWidgets import QGraphicsPixmapItem, QWidget, QLabel, QGraphicsScene, QGraphicsView
import pyqtgraph as pg

from mGesf.drawer import init_container, setup_radar_connection_button, setup_user_port, setup_data_port, \
    setup_datapath_block, setup_information_block, setup_record_button, setup_config_path_block, setup_config_btn, \
    setup_sensor_btn
from utils.img_utils import array_to_colormap_qim

import mGesf.MMW_worker as MMW_worker
from mGesf.drawer import *
import mGesf.config as config


class Control_tab(QWidget):
    """
        main page
            1. control block
                1-1. RLU block
                    1-1-1. Radar block
                        1-1-1-0. Radar frame # todo add frames
                        1-1-1-1. Connection block
                            1-1-1-1-1. Data port block
                            1-1-1-1-2. User port block
                            1-1-1-1-3. Connect button
                        1-1-1-2. Sensor block
                            1-1-1-2-1. Config path block
                            1-1-1-2-2. senor buttons block
                                1-1-1-2-2-1. Send_config Button
                                1-1-1-2-2-2. Start/Stop sensor button
                        1-1-1-3. Runtime view
                        1-1-1-4. Radar record check box
                    1-1-2. Leap block
                        1-1-2-1. Leap connect button block
                        1-1-2-2. Leap runtime view
                        1-1-2-3. Leap record check box
                    1-1-3. UWB block
                        1-1-2-1. UWB connect Button
                        1-1-2-2. UWB runtime view
                        1-1-2-3. UWB record check box
                1-2. Record block
                    1-2-1. Output path text box
                    1-2-2. Record button

            2. information block
                2-1. message
    """

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

        self.will_recording_radar = False
        self.will_recording_leap = False
        self.will_recording_UWB = False

        self.is_recording_radar = False
        self.is_recording_leap = False
        self.is_recording_UWB = False

        # #################### create mmWave layout #################################

        # -------------------- First class --------------------
        # main page
        self.main_page = QtWidgets.QHBoxLayout(self)
        self.setLayout(self.main_page)

        # -------------------- Second class --------------------
        #   1. control block
        #   2. information block

        self.control_block = init_container(parent=self.main_page)
        # ***** information block *****
        self.information_block, self.message = setup_information_block(parent=self.main_page)
        # self.information_block_frame = draw_frame(self.main_page, config.WINDOW_WIDTH / 5, config.WINDOW_HEIGHT)

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
        self.UWB_block = init_container(parent=self.RLU_block, label="Ultra-Wide-Band",
                                        label_position="center")

        # -------------------- fourth class --------------------
        #       1-2. Record block
        #           1-2-1. Output path text box
        #           1-2-2. Record button
        # ***** data_path block *****
        self.data_path_block, self.data_path_textbox = setup_datapath_block(parent=self.record_block)
        # ***** record button *****
        self.record_btn = setup_record_button(parent=self.record_block, function=self.record_btn_action)

        # -------------------- fifth class --------------------
        #           1-1-1. Radar block
        #               1-1-1-0. Radar frame
        #               1-1-1-1. Connection block
        #               1-1-1-2. Sensor block
        #               1-1-1-3. Runtime block
        #               1-1-1-4. Radar record check box
        # self.radar_block_frame = draw_frame(self.RLU_block, config.WINDOW_WIDTH / 3 * (4 / 5),
        #                                    config.WINDOW_HEIGHT * 4 / 5)
        self.radar_connection_block = init_container(parent=self.radar_block, label="Connection")
        self.radar_sensor_block = init_container(parent=self.radar_block, label="Sensor")
        self.radar_runtime_block = init_container(parent=self.radar_block, label="Runtime")
        # self.radar_record_checkbox = setup_check_box(parent=self.radar_block, function=self.radar_clickBox)
        # -------------------- fifth class --------------------
        #               1-1-1-3. Runtime view
        self.radar_runtime_view = self.init_spec_view(parent=self.radar_runtime_block, label="Runtime",
                                                      graph=self.doppler_display)

        # -------------------- fifth class --------------------
        #           1-1-2. Leap block
        #               1-1-2-1. Leap connection button
        #               1-1-2-2. Leap runtime view
        #               1-1-2-3. Leap record check box
        self.leap_connection_btn = setup_sensor_btn(parent=self.leap_block, function=self.leap_connection_btn_action)
        self.leap_runtime_view = self.init_spec_view(parent=self.leap_block, label="Runtime")
        #self.leap_record_checkbox = setup_check_box(parent=self.leap_block, function=self.leap_clickBox)

        # self.leap_block_frame = draw_frame(self.RLU_block, config.WINDOW_WIDTH / 3*(4/5), config.WINDOW_HEIGHT * 4 / 5)
        # -------------------- fifth class --------------------
        #           1-1-3. UWB block
        #               1-1-3-1. UWB connection button
        #               1-1-3-2. UWB runtime view
        self.UWB_connection_btn = setup_sensor_btn(parent=self.UWB_block, function=self.UWB_connection_btn_action)
        self.UWB_runtime_view = self.init_spec_view(parent=self.UWB_block, label="Runtime")
        self.UWB_record_checkbox = setup_check_box(parent=self.UWB_box_container, function=self.UWB_clickBox)

        # self.UWB_block_frame = draw_frame(self.RLU_block, config.WINDOW_WIDTH / 3*(4/5), config.WINDOW_HEIGHT * 4 / 5)

        # -------------------- sixth class --------------------

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

        # -------------------- seventh class --------------------
        #                   1-1-1-2-2. sensor buttons block
        #                       1-1-1-2-2-1. Send_config Button
        #                       1-1-1-2-2-2. Start/Stop sensor button
        self.config_connection_btn = setup_config_btn(parent=self.sensor_buttons_block,
                                                      function=self.send_config_btn_action)
        self.sensor_start_stop_btn = setup_sensor_btn(parent=self.sensor_buttons_block,
                                                      function=self.start_stop_sensor_action)

        self.show()

    def init_spec_view(self, parent, label, graph=None):
        if label:
            ql = QLabel()
            ql.setAlignment(QtCore.Qt.AlignTop)
            ql.setAlignment(QtCore.Qt.AlignCenter)
            ql.setText(label)
            parent.addWidget(ql)

        spc_gv = QGraphicsView()
        parent.addWidget(spc_gv)

        scene = QGraphicsScene(self)
        spc_gv.setScene(scene)
        if graph:
            scene.addItem(graph)
        return scene

    def record_btn_action(self):
        """ 1. Checks user input data path
            2. use default path in no input
            3. record if path valid
        """
        data_path = self.data_path_textbox.text()
        if not data_path:
            data_path = config.data_path

        if self.will_recording_radar:
            if os.path.exists(data_path):
                self.message.setText(config.datapath_set_message + "\nCurrent data path: " + data_path)
                if not self.is_recording_radar:
                    self.is_recording_radar = True
                    self.mmw_worker.record_mmw()
                    self.record_btn.setText("Stop Recording")
                else:
                    self.is_recording_radar = False
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

    def leap_connection_btn_action(self):
        self.message.setText("Leap Connection working...")

    def UWB_connection_btn_action(self):
        self.message.setText("UWB Connection working...")

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
        if self.is_recording_radar:
            self.buffer['mmw']['timestamps'].append(time.time())
            self.buffer['mmw']['ra_profile'].append(data_dict['range_doppler'])

    def radar_clickBox(self, state):

        if state == QtCore.Qt.Checked:
            self.will_recording_radar = True
            self.message.setText(config.radar_box_checked)
        else:
            self.will_recording_radar = False
            self.message.setText(config.radar_box_unchecked)

    def leap_clickBox(self, state):

        if state == QtCore.Qt.Checked:
            self.will_recording_leap = True
            self.message.setText(config.leap_box_checked)
        else:
            self.will_recording_leap = False
            self.message.setText(config.leap_box_unchecked)

    def UWB_clickBox(self, state):

        if state == QtCore.Qt.Checked:
            self.will_recording_UWB = True
            self.message.setText(config.UWB_box_checked)
        else:
            self.will_recording_UWB = False
            self.message.setText(config.UWB_box_unchecked)