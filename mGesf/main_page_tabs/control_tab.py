import os
import pickle
import time
from datetime import datetime
import numpy as np

from PyQt5.QtGui import QPixmap
from PyQt5.QtWidgets import QGraphicsPixmapItem, QWidget, QGraphicsScene, QGraphicsView
import pyqtgraph as pg

from utils.img_utils import array_to_colormap_qim

import mGesf.workers as workers
from utils.GUI_main_window import *
import config as config


class Control_tab(QWidget):
    """
        main page
            1. control block
                1-1. RLU block
                    1-1-1. Radar block
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

    def __init__(self, mmw_worker: workers.MmwWorker, uwb_worker: workers.UWBWorker, leap_worker: workers.LeapWorker,
                 refresh_interval, *args, **kwargs):
        super().__init__()

        # mmW worker
        self.mmw_worker = mmw_worker
        self.mmw_worker.signal_mmw_control_tab.connect(self.control_process_mmw_data)

        # UWB worker
        self.uwb_worker = uwb_worker
        self.uwb_worker.signal_data.connect(self.control_process_uwb_data)

        # LeapMotion worker
        self.leap_worker = leap_worker
        self.leap_worker.signal_leap.connect(self.control_process_leap_data)

        # create the data buffers
        self.buffer = {'mmw': {'timestamps': [], 'range_doppler': [], 'range_azi': [], 'detected_points': []}}
        # add range doppler
        self.doppler_display = QGraphicsPixmapItem()

        self.will_recording_radar = False
        self.will_recording_leap = False
        self.will_recording_UWB = False

        self.is_recording_radar = False
        self.is_recording_leap = False
        self.is_recording_UWB = False

        # Create UWB display

        # #################### create mmWave layout #################################

        # -------------------- First class --------------------
        # main page
        self.main_page = QtWidgets.QHBoxLayout(self)
        self.setLayout(self.main_page)

        # -------------------- Second class --------------------
        # control block
        self.control_block = init_container(parent=self.main_page)

        # -------------------- third class --------------------
        #   1. Control block
        #       1-1. RLU block
        #       1-2. Record block

        self.RLU_block = init_container(parent=self.control_block, vertical=False,
                                        style="background-color:" + config.container_color + ";")
        self.record_block = init_container(parent=self.control_block, label_position="rightbottom",
                                           label="Record",
                                           style="background-color:" + config.container_color + ";")

        # -------------------- fourth class -------------------
        #       1-1. RLU block
        #           1-1-1. Radar block
        #           1-1-2. Leap block
        #           1-1-3. UWB block
        self.radar_block = init_container(parent=self.RLU_block, label="Radar", label_position="center",
                                          style="background-color: " + config.subcontainer_color + ";")
        self.leap_block = init_container(parent=self.RLU_block, label="LeapMotion", label_position="center",
                                         style="background-color: " + config.subcontainer_color + ";")
        self.UWB_block = init_container(parent=self.RLU_block, label="Ultra-Wide-Band",
                                        label_position="center",
                                        style="background-color: " + config.subcontainer_color + ";")

        # -------------------- fourth class --------------------
        #       1-2. Record block
        #           1-2-1. Output path text box
        #           1-2-2. Record button
        # ***** data_path block *****
        self.sub_record_block = init_container(parent=self.record_block,
                                               style="background-color: " + config.subcontainer_color + ";")

        self.data_path_block, self.data_path_textbox = init_inputBox(parent=self.sub_record_block,
                                                                     label=config.control_tab_output_path_label,
                                                                     label_bold=True,
                                                                     default_input=config.data_path_default)
        # ***** record button *****
        self.record_btn = init_button(parent=self.sub_record_block,
                                      label=config.record_btn_start_label,
                                      function=self.record_btn_action)
        # -------------------- fifth class --------------------
        #           1-1-1. Radar block
        #               1-1-1-0. Radar frame
        #               1-1-1-1. Connection block
        #               1-1-1-2. Sensor block
        #               1-1-1-3. Runtime block
        #               1-1-1-4. Radar record check box
        # self.radar_block_frame = draw_boarder(self.RLU_block, config.WINDOW_WIDTH / 3 * (4 / 5),
        #                                       config.WINDOW_HEIGHT * 4 / 5)

        self.radar_connection_block = init_container(parent=self.radar_block, label="Connection",
                                                     style="background-color: " + config.container_color + ";")
        self.radar_sensor_block = init_container(parent=self.radar_block, label="Sensor",
                                                 style="background-color: " + config.container_color + ";")
        self.radar_runtime_view = self.init_spec_view(parent=self.radar_block, label="Runtime",
                                                      graph=self.doppler_display)
        self.radar_record_checkbox = init_checkBox(parent=self.radar_block, function=self.radar_clickBox)

        # -------------------- fifth class --------------------
        #           1-1-2. Leap block
        #               1-1-2-1. Leap connection button
        #               1-1-2-2. Leap runtime view
        #               1-1-2-3. Leap record check box
        self.leap_connection_btn = init_button(parent=self.leap_block,
                                               label=config.sensor_btn_label,
                                               function=self.leap_connection_btn_action)

        self.leap_runtime_view = self.init_spec_view(parent=self.leap_block, label="Runtime")
        self.leap_record_checkbox = init_checkBox(parent=self.leap_block, function=self.leap_clickBox)
        self.leap_scatter = self.init_leap_scatter(parent=self.leap_runtime_view, label="LeapMouse")

        # -------------------- fifth class --------------------
        #           1-1-3. UWB block
        #               1-1-3-1. UWB connection button
        #               1-1-3-2. UWB runtime view
        self.UWB_connection_btn = init_button(parent=self.UWB_block,
                                              label=config.sensor_btn_label,
                                              function=self.UWB_start_btn_action)

        self.runtime_plot_1, self.runtime_plot_2, self.runtime_plot_3, self.runtime_plot_4 = self.init_uwb_line_view(
            parent=self.UWB_block, label="UWB IR")
        self.UWB_record_checkbox = init_checkBox(parent=self.UWB_block, function=self.UWB_clickBox)

        # -------------------- sixth class --------------------

        # ***** ports *****

        self.data_port_block, self.dport_textbox = init_inputBox(parent=self.radar_connection_block,
                                                                 label=config.control_tab_data_port_label,
                                                                 label_bold=True,
                                                                 default_input=config.control_tab_d_port_default)
        self.user_port_block, self.uport_textbox = init_inputBox(parent=self.radar_connection_block,
                                                                 label=config.control_tab_user_port_label,
                                                                 label_bold=True,
                                                                 default_input=config.control_tab_u_port_default)
        # ***** connect button *****
        self.radar_connection_btn = init_button(parent=self.radar_connection_block,
                                                label=config.radar_connection_btn_label,
                                                function=self.radar_connection_btn_action)

        # -------------------- sixth class --------------------
        #               1-1-1-2. Sensor block
        #                   1-1-1-2-1. Config path block
        #                   1-1-1-2-2. senor buttons block
        #                       1. Send_config Button
        #                       2. Start/Stop sensor button

        self.is_valid_config_path, self.config_textbox = setup_configPath_block(parent=self.radar_sensor_block)
        self.sensor_buttons_block = init_container(self.radar_sensor_block, vertical=False)

        # -------------------- seventh class --------------------
        #                   1-1-1-2-2. sensor buttons block
        #                       1-1-1-2-2-1. Send_config Button
        #                       1-1-1-2-2-2. Start/Stop sensor button
        self.config_connection_btn = init_button(parent=self.sensor_buttons_block,
                                                 label=config.send_config_btn_label,
                                                 function=self.send_config_btn_action)

        self.sensor_start_stop_btn = init_button(parent=self.sensor_buttons_block,
                                                 label=config.sensor_btn_label,
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
        spc_gv.setAlignment(QtCore.Qt.AlignCenter)
        if graph:
            scene.addItem(graph)
        # spc_gv.setFixedSize(config.WINDOW_WIDTH/4, config.WINDOW_HEIGHT/4)
        return scene

    def init_uwb_line_view(self, parent, label):
        if label:
            ql = QLabel()
            ql.setAlignment(QtCore.Qt.AlignTop)
            ql.setAlignment(QtCore.Qt.AlignCenter)
            ql.setText(label)
            parent.addWidget(ql)
        line_view = pg.PlotWidget()
        parent.addWidget(line_view)

        # line_view.setXRange(0, 130, padding=0)
        # line_view.setYRange(0, 1, padding=0)

        pen = pg.mkPen(color=(255, 0, 0), width=2)
        runtime_plot_1 = line_view.plot(np.zeros((130, 2)), pen=pen, name="Anchor - Real")
        pen = pg.mkPen(color=(0, 255, 0), width=2)
        runtime_plot_2 = line_view.plot(np.zeros((130, 2)), pen=pen, name="Anchor - Imaginary")
        pen = pg.mkPen(color=(255, 255, 0), width=2)
        runtime_plot_3 = line_view.plot(np.zeros((130, 2)), pen=pen, name="Tag - Real")
        pen = pg.mkPen(color=(0, 255, 255), width=2)
        runtime_plot_4 = line_view.plot(np.zeros((130, 2)), pen=pen, name="Tag - Imaginary")

        return runtime_plot_1, runtime_plot_2, runtime_plot_3, runtime_plot_4

    def init_leap_scatter(self, parent, label):
        if label:
            ql = QLabel()
            ql.setAlignment(QtCore.Qt.AlignTop)
            ql.setAlignment(QtCore.Qt.AlignCenter)
            ql.setText(label)
            parent.addWidget(ql)

        pts_plt = pg.PlotWidget()
        parent.addWidget(pts_plt)
        pts_plt.setXRange(-0.1, 0.1)
        pts_plt.setYRange(1.5, 1.7)
        scatter = pg.ScatterPlotItem(pen=None, symbol='o')
        pts_plt.addItem(scatter)
        return scatter

    def record_btn_action(self):
        """ 1. Checks user input data path
            2. use default path in no input
            3. record if path valid
        """
        data_path = self.data_path_textbox.text()
        if not data_path:
            data_path = config.data_path_default

        if self.will_recording_radar:
            if os.path.exists(data_path):
                print(config.datapath_set_message + "\nCurrent data path: " + data_path)
                if not self.is_recording_radar:
                    self.is_recording_radar = True
                    print('Recording started!')

                    self.record_btn.setText("Stop Recording")
                else:
                    self.is_recording_radar = False
                    self.record_btn.setText("Start Recording")

                    today = datetime.now()
                    pickle.dump(self.buffer, open(os.path.join(config.data_path_default,
                                                               today.strftime("%b-%d-%Y-%H-%M-%S") + '.mgesf'), 'wb'))
                    print('Data save to ' + config.data_path_default)
                    self.reset_buffer()
            else:
                print(config.datapath_invalid_message + "\nCurrent data path: " + data_path)
        elif not (self.will_recording_radar and self.will_recording_leap and self.will_recording_UWB):
            print("No sensor selected. Select at least one to record.")

    def radar_connection_btn_action(self):
        """ 1. Get user entered ports
            2. use default ports in no input
            3. Connect if ports valid
        """
        if self.mmw_worker.is_connected():
            self.mmw_worker.disconnect_mmw()
            self.dport_textbox.setPlaceholderText('default: ' + config.control_tab_d_port_default)
            self.dport_textbox.setPlaceholderText('default: ' + config.control_tab_u_port_default)
            print(config.control_tab_UDport_disconnected_message)
            self.radar_connection_btn.setText('Connect')
        else:
            # TODO: CHECK VALID PORTS
            self.mmw_worker.connect_mmw(uport_name=self.uport_textbox.text(), dport_name=self.dport_textbox.text())
            print(config.control_tab_UDport_connected_message)
            self.radar_connection_btn.setText('Disconnect')

    def leap_connection_btn_action(self):
        print("Leap Connection working...")
        self.leap_worker.start_leap()

    def UWB_start_btn_action(self):
        print("connect to UWB sensor")
        self.uwb_worker.start_uwb()

    def send_config_btn_action(self):
        """ 1. Get user entered config path
            2. use default config path in no input
            3. Send config if valid
        """

        config_path = self.config_textbox[1].text()
        if not config_path:
            config_path = config.control_tab_config_file_path_default

        if os.path.exists(config_path):
            self.is_valid_config_path = True
            print(config.config_set_message + "\nCurrent path: " + config_path)
            self.mmw_worker.send_config(config_path=config_path)
        else:
            self.is_valid_config_path = False
            print(config.config_invalid_message + "\nCurrent path: " + config_path)

    def start_stop_sensor_action(self):
        # TODO: CONNECT WHEN CONFIG PATH VALID
        if self.mmw_worker.is_mmw_running():
            self.sensor_start_stop_btn.setText('Start Sensor')
            self.mmw_worker.stop_mmw()
            print(config.control_tab_stop_sensor_message)
        else:
            self.sensor_start_stop_btn.setText('Stop Sensor')
            self.mmw_worker.start_mmw()
            print(config.control_tab_start_sensor_message)

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
        doppler_qpixmap = doppler_qpixmap.scaled(128, 128, pg.QtCore.Qt.KeepAspectRatio)  # resize spectrogram
        self.doppler_display.setPixmap(doppler_qpixmap)

        # save the data is record is enabled
        # mmw buffer: {'timestamps': [], 'ra_profile': [], 'rd_heatmap': [], 'detected_points': []}
        if self.is_recording_radar:
            ts = time.time()
            try:
                assert data_dict['range_doppler'].shape == config.rd_shape
                assert data_dict['range_azi'].shape == config.ra_shape
            except AssertionError:
                print('Invalid data shape at ' + str(ts) + ', discarding frame.')
                return
            finally:
                self.buffer['mmw']['timestamps'].append(ts)
                # expand spectrogram dimension for channel_first
                self.buffer['mmw']['range_doppler'].append(np.expand_dims(data_dict['range_doppler'], axis=0))
                self.buffer['mmw']['range_azi'].append(np.expand_dims(data_dict['range_azi'], axis=0))
                self.buffer['mmw']['detected_points'].append(data_dict['pts'])

    def control_process_uwb_data(self, data_dict):
        if data_dict['a_frame'] is not None:
            x_samples = list(range(data_dict['a_frame'].shape[0]))
            a_real = data_dict['a_frame'][:, 0]
            # a_img = data_dict['a_frame'][:, 1]
            # t_real = data_dict['t_frame'][:, 0]
            # t_img = data_dict['t_frame'][:, 1]
            print('processing UWB data')

            self.runtime_plot_1.setData(x_samples, a_real, )
        # self.runtime_plot_2.setData(x_samples, a_img,)
        # self.runtime_plot_3.setData(x_samples, t_real,)
        # self.runtime_plot_4.setData(x_samples, t_img,)

        # runtime_plot_2, runtime_plot_3, runtime_plot_4
        # self.UWB_runtime_view.plot(x_samples, a_real)

        # self.UWB_runtime_view.plot(x_samples, a_real, "Anchor - Real", pen=pen)
        # self.UWB_runtime_view.plot(x_samples, a_img, "Anchor - Imaginary", pen=pen)
        # self.UWB_runtime_view.plot(x_samples, t_real, "Tag - Real", pen=pen)
        # self.UWB_runtime_view.plot(x_samples, t_img, "Tag - Imaginary", pen=pen)

    def control_process_leap_data(self, data_dict):
        new_x_pos, new_y_pos = data_dict['leapmouse'][3], data_dict['leapmouse'][4]
        self.leap_scatter.setData([new_x_pos], [new_y_pos])

    def radar_clickBox(self, state):

        if state == QtCore.Qt.Checked:
            self.will_recording_radar = True
            print(config.radar_box_checked)
        else:
            self.will_recording_radar = False
            print(config.radar_box_unchecked)

    def leap_clickBox(self, state):

        if state == QtCore.Qt.Checked:
            self.will_recording_leap = True
            print(config.leap_box_checked)
        else:
            self.will_recording_leap = False
            print(config.leap_box_unchecked)

    def UWB_clickBox(self, state):

        if state == QtCore.Qt.Checked:
            self.will_recording_UWB = True
            print(config.UWB_box_checked)
        else:
            self.will_recording_UWB = False
            print(config.UWB_box_unchecked)

    def reset_buffer(self):
        self.buffer = {'mmw': {'timestamps': [], 'range_doppler': [], 'range_azi': [], 'detected_points': []}}
