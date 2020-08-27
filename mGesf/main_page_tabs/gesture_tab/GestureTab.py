import os
import pickle
import time
from datetime import datetime

from PyQt5.QtCore import pyqtSignal
from PyQt5.QtGui import QPixmap
from PyQt5.QtWidgets import QGraphicsPixmapItem, QWidget, QGraphicsScene, QGraphicsView, QTabWidget
import pyqtgraph as pg
from sklearn.externals._pilutil import imresize

from mGesf import workers, utils
from utils.GUI_main_window import *
import config as config
from mGesf.main_page_tabs.gesture_tab.desktopFingertip.DesktopFingertip import DesktopFingertip
from mGesf.main_page_tabs.gesture_tab.indexPen.idp_main import IndexPen
from mGesf.main_page_tabs.gesture_tab.thuMouse.thm_main import ThuMouse
from utils.GUI_operation_tab import init_xethrux4_runtime_view
from utils.img_utils import array_to_colormap_qim, process_clutter_removed_spectrogram, plot_spectrogram


class GestureTab(QWidget):
    """
    """
    record_signal = pyqtSignal(dict)

    def __init__(self, mmw_worker: workers, leap_worker: workers, xethrux4_worker: workers, *args, **kwargs):
        super().__init__()
        # #################### create layout #################################

        self.will_recording_mmw = False
        self.will_recording_leap = False
        self.will_recording_UWB = False
        self.will_recording_xethrux4 = False
        self.is_recording = False
        self.buffer = None
        self.clear_buffer()
        # -------------------- First class --------------------
        # main page
        self.main_page = QtWidgets.QVBoxLayout(self)
        self.setLayout(self.main_page)

        # -------------------- Second class --------------------
        #   1. runtime feature block
        #   2. ITD （index pen, thumouse, desktop at finger) block

        self.runtime_block = init_container(parent=self.main_page, vertical=False)
        # ***** information block *****
        self.ITD_block = init_container(parent=self.main_page, vertical=False)

        # -------------------- third class --------------------
        #   1. Runtime feature block
        #       1-1. radar runtime block
        #       1-2. leap runtime block
        #       1-3. UWB runtime block
        self.mmw_runtime_block = init_container(parent=self.runtime_block, vertical=True)
        self.leap_runtime_block = init_container(parent=self.runtime_block, vertical=True)
        self.uwb_runtime_block = init_container(parent=self.runtime_block, vertical=True)
        self.xethrux4_runtime_block = init_container(parent=self.runtime_block, vertical=True)

        # -------------------- fourth class -------------------
        #       1-1. radar runtime block
        #           1-1-1. radar runtime graph
        #           1-1-2. radar checkbox
        self.mmw_doppler_display = QGraphicsPixmapItem()
        self.mmw_runtime_view = self.init_spec_view(parent=self.mmw_runtime_block, label="Radar",
                                                    graph=self.mmw_doppler_display)
        self.mmw_record_checkbox = init_checkBox(parent=self.mmw_runtime_block, function=self.radar_clickBox)
        self.mmw_record_checkbox.setChecked(True)
        self.mmw_worker = mmw_worker
        self.mmw_worker.signal_data.connect(self.display_record_mmw_data)

        # -------------------- fourth class -------------------
        #       1-1. leap runtime block
        #           1-1-1. leap runtime graph
        #           1-1-2. leap checkbox

        self.leap_runtime_view = self.init_spec_view(parent=self.leap_runtime_block, label="Leap",
                                                     graph=None)
        self.leap_record_checkbox = init_checkBox(parent=self.leap_runtime_block, function=self.leap_clickBox)

        # -------------------- fifth class -------------------
        #       1-1. UWB runtime block
        #           1-1-1. UWB runtime graph
        #           1-1-2. UWB checkbox
        self.UWB_runtime_view = self.init_spec_view(parent=self.uwb_runtime_block, label="UWB",
                                                    graph=None)
        self.UWB_record_checkbox = init_checkBox(parent=self.uwb_runtime_block, function=self.UWB_clickBox)

        # -------------------- sixth class -------------------
        #       1-1. xeThruX4 runtime block
        #           1-1-1. xeThruX4 runtime graph
        #           1-1-2. xeThruX4 checkbox
        # self.xethrux4_ir_spectrogram_display = QGraphicsPixmapItem()
        self.rf_curve, self.baseband_curve = init_xethrux4_runtime_view(parent=self.xethrux4_runtime_block, label="RF frame")
        # self.xethrux4_runtime_view = self.init_spec_view(parent=self.xethrux4_runtime_block, label="XeThruX4",
        #                                                  graph=self.xethrux4_ir_spectrogram_display)
        self.xethrux4_record_checkbox = init_checkBox(parent=self.xethrux4_runtime_block, function=self.xethrux4_clickBox)
        self.xethrux4_record_checkbox.setChecked(True)
        self.xethrux4_worker = xethrux4_worker
        self.xethrux4_worker.signal_data.connect(self.display_record_xethrux4_data)

        # -------------------- third class --------------------
        #   1. ITD block
        #       1-1. radar runtime block
        #       1-2. leap runtime block
        #       1-3. UWB runtime block

        # Initialize tab screen
        self.record_signal.connect(self.record_signal_action)
        self.tabs = QTabWidget()
        self.tab_idp = IndexPen(self.record_signal, mmw_worker.signal_data)
        self.tab_thm = ThuMouse()
        self.tab_dft = DesktopFingertip()

        self.tabs.addTab(self.tab_idp, config.gesture_index_pen_label)
        self.tabs.addTab(self.tab_thm, config.gesture_thuMouse_label)
        self.tabs.addTab(self.tab_dft, config.gesture_desktop_fingertip_label)

        # Add tabs to main_widget
        self.ITD_block.addWidget(self.tabs)
        # self.setLayout(self.ITD_block)

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

    def radar_clickBox(self, state):
        self.will_recording_mmw = self.mmw_record_checkbox.isChecked()

    def leap_clickBox(self, state):

        self.will_recording_leap = self.leap_record_checkbox.isChecked()

    def UWB_clickBox(self, state):
        self.will_recording_UWB = self.UWB_record_checkbox.isChecked()

    def xethrux4_clickBox(self, state):
        self.will_recording_xethrux4 = self.xethrux4_record_checkbox.isChecked()

    @QtCore.pyqtSlot(dict)
    def display_record_xethrux4_data(self, data_dict):
        if data_dict['frame'] is not None:
            xsamples = list(range(data_dict['frame'].shape[0]))
            rf_frame = data_dict['frame']
            baseband_frame = data_dict['baseband_frame']

            self.rf_curve.setData(xsamples, rf_frame)
            self.baseband_curve.setData(xsamples, baseband_frame)

            if self.is_recording and self.will_recording_xethrux4:
                utils.record_xethrux4_frame(data_dict=data_dict, buffer=self.buffer)

    @QtCore.pyqtSlot(dict)
    def display_record_mmw_data(self, data_dict):
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
        doppler_qpixmap = \
            process_clutter_removed_spectrogram(data_dict['range_doppler_rc'], config.rd_vmax,
                                                config.rd_vmin, config.rd_shape[0],
                                                height=config.rd_controlGestureTab_display_dim,
                                                width=config.rd_controlGestureTab_display_dim) \
                if config.is_plot_mmWave_rc else plot_spectrogram(data_dict['range_doppler'], )
        self.mmw_doppler_display.setPixmap(doppler_qpixmap)

        if self.is_recording and self.will_recording_mmw:
            utils.record_mmw_frame(data_dict=data_dict, buffer=self.buffer)

    def record_signal_action(self, signal: dict):
        if signal['cmd'] == 'start':
            self.is_recording = True
            print('GestureTab: recording STARTED')
        elif signal['cmd'] == 'end':
            self.is_recording = False
            today = datetime.now()
            data_path = os.path.join(self.tab_idp.get_record_data_path(),
                                     today.strftime("%b-%d-%Y-%H-%M-%S") +
                                     '_' + signal['subject_name'] +
                                     '_data.mgesf')
            label_path = os.path.join(self.tab_idp.get_record_data_path(),
                                      today.strftime("%b-%d-%Y-%H-%M-%S") +
                                      '_' + signal['subject_name'] +
                                      '_label.mgesf')
            try:
                print(len(self.buffer['mmw']['timestamps']))
                pickle.dump(self.buffer, open(data_path, 'wb'))
                pickle.dump(signal['label'], open(label_path, 'wb'))
                print('GestureTab: recording ENDED, data saved to ' + self.tab_idp.get_record_data_path())
            except FileNotFoundError:
                print('GestureTab: data path does not exist')  # TODO check path exist before recording starts
        else:
            raise Exception('GestureTab: record_signal_action: unknown signal')
        self.clear_buffer()
        print('record buffer cleared.')

    def clear_buffer(self):
        self.buffer = {'mmw': {'timestamps': [], 'range_doppler': [], 'range_azi': [], 'detected_points': []},
                       'xethrux4': {'timestamps': [], 'frame': [], 'baseband_frame': [], 'clutter_removal_frame': [], 'clutter_removal_baseband_frame': []}}

    def set_fire_tab_signal(self, is_fire_signal):
        if is_fire_signal:
            print('enabled gesture signal') if config.debug else print()
            self.mmw_worker.signal_data.connect(self.display_record_mmw_data)
            self.xethrux4_worker.signal_data.connect(self.display_record_xethrux4_data)
        else:
            try:
                print('disable gesture signal') if config.debug else print()
                self.mmw_worker.signal_data.disconnect(self.display_record_mmw_data)
                self.xethrux4_worker.signal_data.disconnect(self.display_record_xethrux4_data)
            except TypeError:
                pass
