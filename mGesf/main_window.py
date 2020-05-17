import sys

from PyQt5 import uic
from PyQt5.QtCore import QTimer
from PyQt5.QtWidgets import QWidget, QMainWindow, QVBoxLayout, QTabWidget, \
    QHBoxLayout, QFormLayout, QScrollArea
import pyqtgraph as pg
import time
from utils.InformationPane import InformationPane
from utils.iwr6843_utils.mmWave_interface import MmWaveSensorInterface

import mGesf.MMW_worker as MMW_worker

# tabs ======================================
import mGesf.main_page_tabs.control_tab as control_tab
import mGesf.main_page_tabs.radar_tab as radar_tab
import mGesf.main_page_tabs.leap_tab as leap_tab
import mGesf.main_page_tabs.UWB_tab as UWB_tab
import mGesf.main_page_tabs.gesture_tab.gesturetab as gesture_tab
import config as config

# class WorkerSignals(QObject):
#     finished = pyqtSignal()
#     error = pyqtSignal(tuple)
#     result = pyqtSignal(object)
#     progress = pyqtSignal(int)

# TODO add resume function to the stop button
from utils.std_utils import Stream


class MainWindow(QMainWindow):
    def __init__(self, mmw_interface: MmWaveSensorInterface, refresh_interval, data_path, *args, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)
        uic.loadUi('mGesf/resource/ui/MainWindow.ui', self)

        pg.setConfigOption('background', 'w')

        main_layout = self.findChild(QHBoxLayout, 'mainLayout')

        # create the tabs: Control, Radar, Leap, UWB, and Gesture
        self.main_widget = self.findChild(QWidget, 'mainWidget')
        self.table_widget = Tabs(self.main_widget, mmw_interface, refresh_interval, data_path)
        self.setCentralWidget(self.table_widget)
        # create the information black
        # self.info_scroll = self.findChild(QScrollArea, 'infoScroll')
        self.show()


class Tabs(QWidget):
    """A frame contains 4 tabs and their contents"""

    def __init__(self, parent, mmw_interface: MmWaveSensorInterface, refresh_interval, data_path, *args, **kwargs):
        super(QWidget, self).__init__(parent)

        self.layout = QHBoxLayout(self)

        # create threading
        # create a QThread and start the thread that handles
        self.worker_thread = pg.QtCore.QThread(self)
        self.worker_thread.start()

        # worker
        self.mmw_worker = MMW_worker.MmwWorker(mmw_interface)
        self.mmw_worker.moveToThread(self.worker_thread)

        # timer
        self.timer = QTimer()
        self.timer.setInterval(refresh_interval)
        self.timer.timeout.connect(self.ticks)
        self.timer.start()

        # Initialize tab screen

        self.tabs = QTabWidget()
        self.tab1 = control_tab.Control_tab(self.mmw_worker, refresh_interval, data_path)
        self.tab2 = radar_tab.Radar_tab(self.mmw_worker, refresh_interval, data_path)
        self.tab3 = leap_tab.Leap_tab()
        self.tab4 = UWB_tab.UWB_tab()
        self.tab5 = gesture_tab.GestureTab(self.mmw_worker)

        self.tabs.addTab(self.tab1, config.main_window_control_tab_label)
        self.tabs.addTab(self.tab2, config.main_window_radar_tab_label)
        self.tabs.addTab(self.tab3, config.main_window_leap_tab_label)
        self.tabs.addTab(self.tab4, config.main_window_uwb_tab_label)
        self.tabs.addTab(self.tab5, config.main_window_gesture_tab_label)

        # Add tabs to widget
        self.layout.addWidget(self.tabs)
        self.setLayout(self.layout)

        self.info_pane = InformationPane(parent=self.layout)
        sys.stdout = Stream(newText=self.on_print)

    def on_print(self, msg):
        self.info_pane.push(msg)

    def __del__(self):
        sys.stdout = sys.__stdout__  # return control to regular stdout

    @pg.QtCore.pyqtSlot()
    def ticks(self):
        """
        ticks every 'refresh' milliseconds
        """
        self.mmw_worker.tick_signal.emit()  # signals the worker to run process_on_tick
