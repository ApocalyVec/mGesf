import os
import pickle
import time
from datetime import datetime
import numpy as np

from PyQt5.QtGui import QPixmap
from PyQt5.QtWidgets import QGraphicsPixmapItem, QWidget, QGraphicsScene, QGraphicsView, QTabWidget
import pyqtgraph as pg

from mGesf.main_page_tabs.XeThruX4ControlPane import XeThruX4ControlPane
from utils.GUI_operation_tab import init_slider_bar_box, init_smooth_slider
from utils.data_utils import scale_rd_spectrogram
from utils.img_utils import array_to_colormap_qim, process_clutter_removed_spectrogram, plot_spectrogram
from utils.img_utils import array_to_colormap_qim, array_to_colormap_qim_leap
from utils.img_utils import array_to_colormap_qim, array_to_colormap_qim_leap

import mGesf.workers as workers
from utils.GUI_main_window import *
import config as config

import mGesf.exceptions as exceptions


class ControlTab(QWidget):
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
                 Xe4Thru_worker: workers.Xe4ThruWorker,
                 refresh_interval, *args, **kwargs):
        super().__init__()

        # mmW worker
        self.mmw_worker = mmw_worker
        self.mmw_worker.signal_data.connect(self.control_process_mmw_data)

        # # UWB worker
        # self.uwb_worker = uwb_worker
        # self.uwb_worker.signal_data.connect(self.control_process_uwb_data)

        # LeapMotion worker
        self.leap_worker = leap_worker
        self.leap_worker.signal_leap.connect(self.control_process_leap_data)
        self.leap_display = QGraphicsPixmapItem()


        # create the data buffers
        self.buffer = {'mmw': {'timestamps': [], 'range_doppler': [], 'range_azi': [], 'detected_points': []}}
        # add range doppler
        self.doppler_display = QGraphicsPixmapItem()

        self.will_record = []
        self.is_recording = []

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
        self.mmw_block = init_container(parent=self.RLU_block, label="mmWave Radar", label_position="center",
                                        style="background-color: " + config.subcontainer_color + ";")
        self.leap_block = init_container(parent=self.RLU_block, label="LeapMotion Camera", label_position="center",
                                         style="background-color: " + config.subcontainer_color + ";")
        # self.UWB_block = init_container(parent=self.RLU_block, label="UWB Antenna",
        #                                 label_position="center",
        #                                 style="background-color: " + config.subcontainer_color + ";")

        # UWB radar block is newly added, so the whole block is here.
        # uwb container
        self.XeThruX4_block = init_container(parent=self.RLU_block, label="XeThruX4 Radar",
                                             label_position="center",
                                             style="background-color: " + config.subcontainer_color + ";")
        #   - device number: label
        self.device_number_box = QLabel("Device number: 1")
        self.XeThruX4_block.addWidget(self.device_number_box)
        #   - tabs
        self.XeThruX4tabs = QTabWidget()
        self.XeThruX4tabs.addTab(XeThruX4ControlPane(Xe4Thru_worker), "XeThruX4 Radar 1")
        self.XeThruX4_block.addWidget(self.XeThruX4tabs)
        # TODO add uwb radar runtime view here
        self.XeThruX4_checkbox = init_checkBox(parent=self.XeThruX4_block, label='record XeThruX4?',
                                               function=self.XeThruX4_clickBox, )

        #   - runtime container

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

        self.mmw_connection_block = init_container(parent=self.mmw_block, label="Connection",
                                                   style="background-color: " + config.container_color + ";")
        self.mmw_sensor_block = init_container(parent=self.mmw_block, label="Sensor",
                                               style="background-color: " + config.container_color + ";")
        self.mmw_rc_block = init_container(parent=self.mmw_block, label="Clutter Removal",
                                           style="background-color: " + config.container_color + ";")
        self.rc_rd_csr_slider = init_smooth_slider(self.mmw_rc_block, 0, 100, 5,
                                                   label='Doppler Signal Clutter Ratio', onChange_func=self.set_mmw_rc_rd_csr)
        self.rc_rd_csr_slider.setValue(config.gui_mmw_rd_rc_csr_default)
        self.rc_ra_csr_slider = init_smooth_slider(self.mmw_rc_block, 0, 100, 5,
                                                   label='Azimuth Signal Clutter Ratio', onChange_func=self.set_mmw_rc_ra_csr)
        self.rc_ra_csr_slider.setValue(config.gui_mmw_ra_rc_csr_default)

        self.mmw_display_rc_checkbox = init_checkBox(parent=self.mmw_rc_block, label='Remove Doppler Clutter',
                                                     function=self.mmw_rc_cb_clicked)
        self.mmw_display_rc_checkbox.setChecked(config.is_plot_mmWave_rc)

        self.mmw_runtime_view = self.init_spec_view(parent=self.mmw_block, label="Runtime",
                                                    graph=self.doppler_display)

        self.mmw_record_checkbox = init_checkBox(parent=self.mmw_block, label='record mmWave?',
                                                 function=self.mmw_clickBox)

        # -------------------- fifth class --------------------
        #           1-1-2. Leap block
        #               1-1-2-1. Leap connection button
        #               1-1-2-2. Leap runtime view
        #               1-1-2-3. Leap record check box
        self.leap_connection_btn = init_button(parent=self.leap_block,
                                               label=config.sensor_btn_label,
                                               function=self.leap_connection_btn_action)

        self.leap_runtime_view = self.init_spec_view(parent=self.leap_block, label="Runtime", graph=self.leap_display)
        self.leap_record_checkbox = init_checkBox(parent=self.leap_block, function=self.leap_clickBox)
        # self.leap_scatter = self.init_leap_scatter(parent=self.leap_runtime_view, label="LeapMouse")

        # -------------------- fifth class --------------------
        #           1-1-3. UWB block
        #               1-1-3-1. UWB connection button
        #               1-1-3-2. UWB runtime view

        # self.uwb_connection_block = init_container(parent=self.UWB_block, label="Connection: ",
        #                                            style="background-color: " + config.container_color + ";")
        #
        # self.uwb_anchor_block, self.tag_port_textbox = init_inputBox(parent=self.uwb_connection_block,
        #                                                              label=config.uwb_tag_port,
        #                                                              label_bold=True,
        #                                                              default_input=config.uwb_tag_default_port)
        #
        # self.uwb_tag_block, self.anchor_port_textbox = init_inputBox(parent=self.uwb_connection_block,
        #                                                              label=config.uwb_anchor_port,
        #                                                              label_bold=True,
        #                                                              default_input=config.uwb_anchor_default_port)

        # self.UWB_tag_connection_btn = init_button(parent=self.uwb_connection_block,
        #                                           label="Connect uwb tag",
        #                                           function=self.uwb_tag_connection_btn_action)
        #
        # self.UWB_anchor_connection_btn = init_button(parent=self.uwb_connection_block,
        #                                              label="Connect uwb anchor",
        #                                              function=self.uwb_anchor_connection_btn_action)

        # #     UWB sensor block
        # self.uwb_sensor_block = init_container(parent=self.UWB_block, label="Sensor",
        #                                        style="background-color: " + config.container_color + ";")
        #
        # self.UWB_sensor_start_btn = init_button(parent=self.uwb_sensor_block,
        #                                         label=config.sensor_btn_label,
        #                                         function=self.UWB_start_btn_action)
        #
        # self.runtime_plot_1, self.runtime_plot_2, self.runtime_plot_3, self.runtime_plot_4 = self.init_uwb_line_view(
        #     parent=self.UWB_block, label="UWB IR")
        # self.UWB_record_checkbox = init_checkBox(parent=self.UWB_block, function=self.UWB_clickBox)

        # -------------------- sixth class --------------------

        # ***** ports *****

        self.data_port_block, self.dport_textbox = init_inputBox(parent=self.mmw_connection_block,
                                                                 label=config.control_tab_data_port_label,
                                                                 label_bold=True,
                                                                 default_input=config.control_tab_d_port_default)
        self.user_port_block, self.uport_textbox = init_inputBox(parent=self.mmw_connection_block,
                                                                 label=config.control_tab_user_port_label,
                                                                 label_bold=True,
                                                                 default_input=config.control_tab_u_port_default)
        # ***** connect button *****
        self.radar_connection_btn = init_button(parent=self.mmw_connection_block,
                                                label=config.connection_btn_label,
                                                function=self.radar_connection_btn_action)

        # -------------------- sixth class --------------------
        #               1-1-1-2. Sensor block
        #                   1-1-1-2-1. Config path block
        #                   1-1-1-2-2. senor buttons block
        #                       1. Send_config Button
        #                       2. Start/Stop sensor button

        self.is_valid_config_path, self.config_textbox = setup_configPath_block(parent=self.mmw_sensor_block)
        self.sensor_buttons_block = init_container(self.mmw_sensor_block, vertical=False)

        self.config_connection_btn = init_button(parent=self.sensor_buttons_block,
                                                 label=config.send_config_btn_label,
                                                 function=self.send_config_btn_action)

        self.sensor_start_stop_btn = init_button(parent=self.sensor_buttons_block,
                                                 label=config.sensor_btn_label,
                                                 function=self.start_stop_sensor_action)

        self.show()

    def set_mmw_rc_rd_csr(self):
        self.mmw_worker.set_rd_csr(self.rc_rd_csr_slider.value() / 100)

    def set_mmw_rc_ra_csr(self):
        self.mmw_worker.set_ra_csr(self.rc_ra_csr_slider.value() / 100)

    def mmw_rc_cb_clicked(self):
        config.is_plot_mmWave_rc = self.mmw_display_rc_checkbox.isChecked()

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

        if "radar" in self.will_record:
            if os.path.exists(data_path):
                print(config.datapath_set_message + "\nCurrent data path: " + data_path)
                if "radar" not in self.is_recording:
                    self.is_recording.append("radar")
                    print('Recording started!')

                    self.record_btn.setText("Stop Recording")
                else:
                    self.is_recording.remove('radar')
                    self.record_btn.setText("Start Recording")

                    today = datetime.now()
                    pickle.dump(self.buffer, open(os.path.join(data_path,
                                                               today.strftime("%b-%d-%Y-%H-%M-%S") + '.mgesf'), 'wb'))
                    print('Data save to ' + config.data_path_default)
                    self.reset_buffer()
            else:
                print(config.datapath_invalid_message + "\nCurrent data path: " + data_path)
        elif not self.will_record:
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

    def uwb_tag_connection_btn_action(self):

        if not (self.uwb_worker._uwb_interface_tag is None):
            try:
                self.uwb_worker._uwb_interface_tag.connect_virtual_port(self.tag_port_textbox.text())
                print("uwb tag connected")
            except exceptions.PortsNotSetUpError:
                print('UWB COM ports are not set up, connect to the sensor prior to start the sensor')

    def uwb_anchor_connection_btn_action(self):

        if not (self.uwb_worker._uwb_interface_anchor is None):
            try:
                self.uwb_worker._uwb_interface_anchor.connect_virtual_port(self.anchor_port_textbox.text())

                print("uwb anchor connected")
            except exceptions.PortsNotSetUpError:
                print('UWB COM ports are not set up, connect to the sensor prior to start the sensor')

    def UWB_start_btn_action(self):
        print("connect to UWB sensor")
        if self.uwb_worker._is_running is True:
            self.uwb_worker._is_running = False
            self.UWB_sensor_start_btn.setText("Start UWB")
        else:
            self.uwb_worker.start_uwb()
            self.UWB_sensor_start_btn.setText("Stop UWB")

        # self.uwb_worker.start_uwb()

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
            self.start_sensor_ui_update()
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
            self.start_sensor_ui_update()
            self.mmw_worker.start_mmw()

    def start_sensor_ui_update(self):
        self.sensor_start_stop_btn.setText('Stop Sensor')
        print(config.control_tab_start_sensor_message)

    @QtCore.pyqtSlot(dict)
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
        # self.rd_maxes = np.append(self.rd_maxes, np.max(data_dict['range_doppler']))
        # self.rd_mins = np.append(self.rd_mins, np.min(data_dict['range_doppler']))
        doppler_qpixmap = \
            process_clutter_removed_spectrogram(data_dict['range_doppler_rc'], config.rd_vmax,
                                                config.rd_vmin, config.rd_shape[0],
                                                height=config.rd_controlGestureTab_display_dim,
                                                width=config.rd_controlGestureTab_display_dim) \
                if config.is_plot_mmWave_rc else plot_spectrogram(data_dict['range_doppler'], )
        self.doppler_display.setPixmap(doppler_qpixmap)

        # save the data is record is enabled
        if 'radar' in self.is_recording:
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

    # def control_process_uwb_data(self, data_dict):
    #     if data_dict['a_frame'] is not None:
    #         x_samples = list(range(data_dict['a_frame'].shape[0]))
    #         a_real = data_dict['a_frame'][:, 0]
    #         a_img = data_dict['a_frame'][:, 1]
    #         # t_real = data_dict['t_frame'][:, 0]
    #         # t_img = data_dict['t_frame'][:, 1]
    #         # print('processing UWB data')
    #
    #         self.runtime_plot_1.setData(x_samples, a_real, )
    #         self.runtime_plot_2.setData(x_samples, a_img, )
    #         # self.runtime_plot_3.setData(x_samples, t_real,)
    #         # self.runtime_plot_4.setData(x_samples, t_img,)

    # runtime_plot_2, runtime_plot_3, runtime_plot_4
    # self.UWB_runtime_view.plot(x_samples, a_real)

    # self.UWB_runtime_view.plot(x_samples, a_real, "Anchor - Real", pen=pen)
    # self.UWB_runtime_view.plot(x_samples, a_img, "Anchor - Imaginary", pen=pen)
    # self.UWB_runtime_view.plot(x_samples, t_real, "Tag - Real", pen=pen)
    # self.UWB_runtime_view.plot(x_samples, t_img, "Tag - Imaginary", pen=pen)

    def control_process_leap_data(self, data_dict):
        # new_x_pos, new_y_pos = data_dict['leapmouse'][3], data_dict['leapmouse'][4]
        # self.leap_scatter.setData([new_x_pos], [new_y_pos])
        leap_image_heatmap_qim = array_to_colormap_qim_leap(data_dict['image'])
        leap_image_qpixmap = QPixmap(leap_image_heatmap_qim)
        leap_image_qpixmap = leap_image_qpixmap.scaled(128, 128, pg.QtCore.Qt.KeepAspectRatio)  # resize spectrogram
        self.leap_display.setPixmap(leap_image_qpixmap)

    def mmw_clickBox(self, state):

        if state == QtCore.Qt.Checked:
            self.will_record.append("radar")
            print(config.radar_box_checked)
        else:
            self.will_record.remove("radar")
            print(config.radar_box_unchecked)

    def leap_clickBox(self, state):

        if state == QtCore.Qt.Checked:
            self.will_record.append("leap")
            print(config.leap_box_checked)
        else:
            self.will_record.remove("leap")
            print(config.leap_box_unchecked)

    def XeThruX4_clickBox(self, state):

        if state == QtCore.Qt.Checked:
            self.will_record.append("XeThruX4")
            print("XeThruX4 checked")
        else:
            self.will_record.remove("XeThruX4")
            print("XeThruX4 unchecked")

    def UWB_clickBox(self, state):

        if state == QtCore.Qt.Checked:
            self.will_record.append("uwb")
            print(config.UWB_box_checked)
        else:
            self.will_record.remove("uwb")
            print(config.UWB_box_unchecked)

    def reset_buffer(self):
        self.buffer = config.init_buffer

    def set_fire_tab_signal(self, is_fire_signal):
        if is_fire_signal:
            print('enabled control signal') if config.debug else print()
            self.mmw_worker.signal_data.connect(self.control_process_mmw_data)
            [self.XeThruX4tabs.widget(i).Xe4Thru_worker.signal_data.connect(
                self.XeThruX4tabs.widget(i).control_process_xethru_data) for i in range(self.XeThruX4tabs.count())]
        else:
            try:
                print('disable control signal') if config.debug else print()
                self.mmw_worker.signal_data.disconnect(self.control_process_mmw_data)
                [self.XeThruX4tabs.widget(i).Xe4Thru_worker.signal_data.disconnect(
                    self.XeThruX4tabs.widget(i).control_process_xethru_data) for i in range(self.XeThruX4tabs.count())]
            except TypeError:
                pass
