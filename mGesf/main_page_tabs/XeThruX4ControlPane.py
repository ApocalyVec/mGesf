import re

from PyQt5.QtWidgets import QWidget, QVBoxLayout, QLabel
from PyQt5 import QtWidgets, QtCore
import config as config
from mGesf import workers
from utils.GUI_main_window import init_container, init_combo_box, init_inputBox, init_checkBox, init_button
from utils.GUI_operation_tab import init_slider_bar_box, init_xethrux4_runtime_view
import pyqtgraph as pg
from utils.XeThru_utils.xeThruX4_algorithm import *

def init_view(label):
    vl = QtWidgets.QVBoxLayout()
    ql = QLabel()
    ql.setAlignment(QtCore.Qt.AlignCenter)
    ql.setText(label)
    vl.addWidget(ql)
    return vl


class XeThruX4ControlPane(QWidget):
    def __init__(self, Xe4Thru_worker: workers.Xe4ThruWorker):
        super().__init__()

        self.figure_gl = QtWidgets.QGridLayout()

        # runtime data visual
        self.Xe4Thru_worker = Xe4Thru_worker
        self.Xe4Thru_worker.signal_data.connect(self.control_process_xethru_data)

        # default range
        self.range_min = 0
        self.range_max = 0.4

        # for checking only one freq box
        self._toggle = None
        self.state = ['idle']  # see the docstring of self.update_state for details

        self.background = QVBoxLayout(self)
        self.setLayout(self.background)

        self.main_page = init_container(parent=self.background, vertical=True,
                                        style="background-color:" + config.container_color + ";")
        #       - device (combo box)
        self.device = init_combo_box(parent=self.main_page,
                                     label="Device",
                                     item_list=["X4M300", "X4M200", "X4M03"])
        self.device.activated[str].connect(self.device_onChanged)
        #       - port (input box)
        self.XeThruX4_port_block, self.XeThruX4_port_textbox = init_inputBox(parent=self.main_page,
                                                                             label="Port (device_name): ",
                                                                             label_bold=True,
                                                                             default_input=config.xethrux4_default_com_port)

        self.freq_block = init_container(parent=self.main_page,
                                         label="Frequency Band",
                                         vertical=False)
        #       - frequency band (check box)
        self.low_freq_checkbox = init_checkBox(parent=self.freq_block,
                                               label="Low (7.290 GHz)",
                                               function=self.low_freq_action)
        self.high_freq_checkbox = init_checkBox(parent=self.freq_block,
                                                label="High (8.748 GHz)",
                                                function=self.high_freq_action)
        self.low_freq_checkbox.setChecked(True)

        #       - range (input boxes)
        self.range_container = init_container(parent=self.main_page,
                                              label="Range (m) [0.5 ~ 3]",
                                              label_bold=True,
                                              vertical=False)

        self.min_range_block, self.min_range_textbox = init_inputBox(parent=self.range_container,
                                                                     label="Min:",
                                                                     label_bold=False,
                                                                     default_input="-0.1")
        self.min_range_textbox.textChanged.connect(self.check_range)
        self.max_range_block, self.max_range_textbox = init_inputBox(parent=self.range_container,
                                                                     label="Max:",
                                                                     label_bold=False,
                                                                     default_input="0.4")
        self.max_range_textbox.textChanged.connect(self.check_range)

        #       - fps ( bar)
        self.fps_block, self.fps_slider_view = init_slider_bar_box(self.main_page,
                                                                   label="FPS",
                                                                   vertical=True,
                                                                   label_bold=True,
                                                                   min_value=10,
                                                                   max_value=40)
        self.fps_slider_view.setValue(config.xethrux4_default_fps)

        # #       - check box
        # self.baseband_block = init_container(parent=self.main_page,
        #                                      label="Baseband",
        #                                      vertical=True)
        # self.baseband_checkbox = init_checkBox(parent=self.baseband_block,
        #                                        function=self.baseband_checkbox_function)
        #       - two buttons

        self.buttons_block = init_container(self.main_page, vertical=False)
        self.start_stop__btn = init_button(parent=self.buttons_block,
                                           label="Start uwb radar",
                                           function=self.start_stop_btn_action)
        self.reset_btn = init_button(parent=self.buttons_block,
                                     label="Reset to default",
                                     function=self.reset_btn_action)

        self.rf_curve, self.baseband_curve = init_xethrux4_runtime_view(parent=self.main_page, label="RF frame")

        self.show()

    def check_range(self):

        self.range_min = re.findall("\d+\.\d+", self.min_range_textbox.text())
        self.range_max = re.findall("\d+\.\d+", self.max_range_textbox.text())

        if self.range_min >= self.range_max:
            print("Range_min >= range_max.")

    def low_freq_action(self):
        if self.low_freq_checkbox.isChecked():
            self.update_state("freq_low")
            self._toggle = True
            self.high_freq_checkbox.setChecked(not self._toggle)
        else:
            self.update_state('not_freq_low')
            self._toggle = not self._toggle
        return

    def high_freq_action(self):
        if self.high_freq_checkbox.isChecked():
            self.update_state("freq_high")
            self._toggle = True
            self.low_freq_checkbox.setChecked(not self._toggle)
        else:
            self.update_state('not_freq_high')
            self._toggle = not self._toggle
        return

    def update_state(self, act):
        """
        update the current state based on action
        The working states, as oppose to 'idle' include that of 'pending', 'testing', 'countingDown', 'writing'
        @param act:
        """

        # check the checkbox logic
        if act in ['follow', 'not_follow', 'locate', 'not_locate']:
            self.check_locate_follow_logic(act)
        # test/record logic
        print("update function not implemented")

    def check_locate_follow_logic(self, act):
        """
        can only choose one
        :return:
        """
        if act == 'freq_low':
            # if locate is chosen, remove it
            if 'freq_high' in self.state:
                self.state.remove('freq_high')
            self.state.append(act)

        elif act == 'not_freq_low':
            if 'freq_low' in self.state:
                self.state.remove('freq_low')

        elif act == 'freq_high':
            if 'freq_low' in self.state:
                self.state.remove('freq_low')
            self.state.append(act)

        elif act == 'freq_high':
            if 'freq_high' in self.state:
                self.state.remove('freq_high')

    def baseband_checkbox_function(self):
        print('Baseband checked. Function not implemented...')


    def start_stop_btn_action(self):

        self.start_stop__btn.blockSignals(True)

        if self.Xe4Thru_worker._is_running:
            # self.Xe4Thru_worker.stop_sensor()
            self.Xe4Thru_worker.stop_sensor()
            print("disconnect " + self.device.currentText())
            self.start_stop__btn.setText("Start uwb radar")

        else:
            def is_number(s):
                try:
                    float(s)
                    return True
                except ValueError:
                    return False

            device_name = self.XeThruX4_port_textbox.text()
            min_range = self.min_range_textbox.text()
            max_range = self.max_range_textbox.text()
            center_frequency = None
            if self.low_freq_checkbox.isChecked():
                center_frequency = 3
            if self.high_freq_checkbox.isChecked():
                center_frequency = 4
            fps = self.fps_slider_view.slider.value()

            if center_frequency is not None and is_number(min_range) and is_number(max_range):
                min_range = float(min_range)
                max_range = float(max_range)
                if min_range < max_range:
                    self.Xe4Thru_worker.start_sensor(device_name=device_name,
                                                     min_range=min_range,
                                                     max_range=max_range,
                                                     center_frequency=center_frequency,
                                                     fps=fps,
                                                     baseband=False)
                    print("connect" + self.device.currentText())
                    self.start_stop__btn.setText("Stop uwb radar")
                else:
                    print("min and max range error")

            else:
                print("Please check your input")

        self.start_stop__btn.blockSignals(False)

    def reset_btn_action(self):
        #reset to defalut
        self.XeThruX4_port_textbox.setText("COM8")
        self.low_freq_checkbox.setChecked(True)
        self.min_range_textbox.setText("-0.1")
        self.max_range_textbox.setText("0.4")
        self.fps_slider_view.setValue(config.xethrux4_default_fps)


        self.state.clear()
        print('reset button clicked. Function not implemented...')

    def device_onChanged(self):
        print("conbobox selection changed. Function not implemented..")

    @QtCore.pyqtSlot(dict)
    def control_process_xethru_data(self, data_dict):
        if data_dict['frame'] is not None:
            xsamples = list(range(data_dict['frame'].shape[0]))
            rf_frame = data_dict['frame']
            baseband_frame = data_dict['baseband_frame']

            self.rf_curve.setData(xsamples, rf_frame)
            self.baseband_curve.setData(xsamples, baseband_frame)

