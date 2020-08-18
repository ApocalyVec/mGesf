import time

from PyQt5.QtGui import QPixmap
from PyQt5.QtWidgets import QGraphicsPixmapItem, QWidget, QGraphicsScene, QGraphicsView, QTabWidget
import pyqtgraph as pg

from mGesf import workers
from utils.GUI_main_window import *
import config as config
from mGesf.main_page_tabs.gesture_tab.desktopFingertip.DesktopFingertip import DesktopFingertip
from mGesf.main_page_tabs.gesture_tab.indexPen.idp_main import IndexPen
from mGesf.main_page_tabs.gesture_tab.thuMouse.thm_main import ThuMouth
from utils.img_utils import array_to_colormap_qim


class GestureTab(QWidget):
    """
    """

    def __init__(self, mmw_worker: workers, leap_worker: workers, xethrux4_worker: workers, *args, **kwargs):
        super().__init__()
        # #################### create layout #################################

        self.will_recording_radar = False
        self.will_recording_leap = False
        self.will_recording_UWB = False
        self.will_recording_xethrux4 = False

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
        self.radar_runtime_block = init_container(parent=self.runtime_block, vertical=True)
        self.leap_runtime_block = init_container(parent=self.runtime_block, vertical=True)
        self.uwb_runtime_block = init_container(parent=self.runtime_block, vertical=True)
        self.xethrux4_runtime_block = init_container(parent=self.runtime_block, vertical=True)

        # -------------------- fourth class -------------------
        #       1-1. radar runtime block
        #           1-1-1. radar runtime graph
        #           1-1-2. radar checkbox
        self.mmw_doppler_display = QGraphicsPixmapItem()
        self.radar_runtime_view = self.init_spec_view(parent=self.radar_runtime_block, label="Radar",
                                                      graph=self.mmw_doppler_display)
        self.radar_record_checkbox = init_checkBox(parent=self.radar_runtime_block, function=self.radar_clickBox)
        mmw_worker.signal_mmw_gesture_tab.connect(self.display_mmw_data)

        # -------------------- fourth class -------------------
        #       1-1. leap runtime block
        #           1-1-1. leap runtime graph
        #           1-1-2. leap checkbox

        self.leap_runtime_view = self.init_spec_view(parent=self.leap_runtime_block, label="Leap",
                                                     graph=None)
        self.leap_record_checkbox = init_checkBox(parent=self.leap_runtime_block, function=self.radar_clickBox)

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
        xethrux4_worker.signal_data.connect(self.display_xethrux4_data)
        self.xethrux4_ir_spectrogram_display = QGraphicsPixmapItem()
        self.xethrux4_runtime_view = self.init_spec_view(parent=self.xethrux4_runtime_block, label="XeThruX4",
                                                         graph=self.xethrux4_ir_spectrogram_display)
        self.xethrux4_record_checkbox = init_checkBox(parent=self.xethrux4_runtime_block, function=self.UWB_clickBox)

        # -------------------- third class --------------------
        #   1. ITD block
        #       1-1. radar runtime block
        #       1-2. leap runtime block
        #       1-3. UWB runtime block

        # Initialize tab screen
        self.tabs = QTabWidget()
        self.tab1 = IndexPen(mmw_worker.signal_mmw_gesture_tab)
        self.tab2 = ThuMouth()
        self.tab3 = DesktopFingertip()

        self.tabs.addTab(self.tab1, config.gesture_index_pen_label)
        self.tabs.addTab(self.tab2, config.gesture_thuMouse_label)
        self.tabs.addTab(self.tab3, config.gesture_desktop_fingertip_label)

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

        if state == QtCore.Qt.Checked:
            self.will_recording_radar = True
            # self.message.setText(config.radar_box_checked)
        else:
            self.will_recording_radar = False
            # self.message.setText(config.radar_box_unchecked)

    def leap_clickBox(self, state):

        if state == QtCore.Qt.Checked:
            self.will_recording_leap = True
            # self.message.setText(config.leap_box_checked)
        else:
            self.will_recording_leap = False
            # self.message.setText(config.leap_box_unchecked)

    def UWB_clickBox(self, state):

        if state == QtCore.Qt.Checked:
            self.will_recording_UWB = True
            # self.message.setText(config.UWB_box_checked)
        else:
            self.will_recording_UWB = False
            # self.message.setText(config.UWB_box_unchecked)

    def xethrux4_clickBox(self, state):
        if state == QtCore.Qt.Checked:
            self.will_recording_xethrux4 = True
            # self.message.setText(config.UWB_box_checked)
        else:
            self.will_recording_xethrux4 = False
            # self.message.setText(config.UWB_box_unchecked)

    def display_xethrux4_data(self, data_dict):
        ir_heatmap_qim = array_to_colormap_qim(data_dict['ir_spectrogram'])
        ir_qpixmap = QPixmap(ir_heatmap_qim)
        ir_qpixmap = ir_qpixmap.scaled(128, 128, pg.QtCore.Qt.KeepAspectRatio)  # resize spectrogram
        self.xethrux4_ir_spectrogram_display.setPixmap(ir_qpixmap)

    def display_mmw_data(self, data_dict):
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
        self.mmw_doppler_display.setPixmap(doppler_qpixmap)

        # save the data is record is enabled
        # mmw buffer: {'timestamps': [], 'ra_profile': [], 'rd_heatmap': [], 'detected_points': []}
        # if self.is_recording_radar:
        #     ts = time.time()
        #     try:
        #         assert data_dict['range_doppler'].shape == config.rd_shape
        #         assert data_dict['range_azi'].shape == config.ra_shape
        #     except AssertionError:
        #         print('Invalid data shape at ' + str(ts) + ', discarding frame.')
        #         return
        #     finally:
        #         self.buffer['mmw']['timestamps'].append(ts)
        #         # expand spectrogram dimension for channel_first
        #         self.buffer['mmw']['range_doppler'].append(np.expand_dims(data_dict['range_doppler'], axis=0))
        #         self.buffer['mmw']['range_azi'].append(np.expand_dims(data_dict['range_azi'], axis=0))
        #         self.buffer['mmw']['detected_points'].append(data_dict['pts'])
