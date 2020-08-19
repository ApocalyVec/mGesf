import sys

from PyQt5 import uic
from PyQt5.QtCore import QTimer
from PyQt5.QtWidgets import QWidget, QMainWindow, QVBoxLayout, QTabWidget, \
    QHBoxLayout, QFormLayout, QScrollArea
import pyqtgraph as pg
import time

from mGesf.main_page_tabs.ControlTab import ControlTab
from mGesf.main_page_tabs.RadarTab import RadarTab
from mGesf.main_page_tabs.LeapTab import LeapTab
from mGesf.main_page_tabs.XeThruX4Tab import XeThruX4Tab
from mGesf.main_page_tabs.UWBTab import UWBTab
from mGesf.main_page_tabs.gesture_tab.GestureTab import GestureTab

from utils.InformationPane import InformationPane
from utils.iwr6843_utils.mmWave_interface import MmWaveSensorInterface

import mGesf.workers as workers

# tabs ======================================
import config as config

# class WorkerSignals(QObject):
#     finished = pyqtSignal()
#     error = pyqtSignal(tuple)
#     result = pyqtSignal(object)
#     progress = pyqtSignal(int)

# TODO add resume function to the stop button
from utils.std_utils import Stream


class MainWindow(QMainWindow):
    def __init__(self, mmw_interface: MmWaveSensorInterface, leap_interface,
                 uwb_interface_anchor, uwb_interface_tag,
                 xeThruX4SensorInterface,
                 refresh_interval, data_path, *args, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)
        uic.loadUi('mGesf/resource/ui/MainWindow.ui', self)
        pg.setConfigOption('background', 'w')
        self.title = 'Micro-gesture Sensor-fusion'

        main_layout = self.findChild(QHBoxLayout, 'mainLayout')

        # create the tabs: Control, Radar, Leap, UWB, and Gesture
        self.main_widget = self.findChild(QWidget, 'mainWidget')
        self.table_widget = Tabs(self.main_widget, mmw_interface, leap_interface,
                                 uwb_interface_anchor, uwb_interface_tag,
                                 xeThruX4SensorInterface,
                                 refresh_interval, data_path)
        self.setCentralWidget(self.table_widget)
        # create the information black
        # self.info_scroll = self.findChild(QScrollArea, 'infoScroll')
        self.show()


class Tabs(QWidget):
    """A frame contains 4 tabs and their contents"""

    def __init__(self, parent, mmw_interface: MmWaveSensorInterface, leap_interface,
                 uwb_interface_anchor, uwb_interface_tag,
                 xeThruX4Sensor_interface,
                 refresh_interval, data_path, *args, **kwargs):
        super(QWidget, self).__init__(parent)

        self.layout = QHBoxLayout(self)

        # create threading; create a QThread and start the thread that handles; worker for sensors
        self.worker_threads = None
        self.init_sensor_threads()  # create threads on which the sensor worker resides
        self.workers = None
        # create workers to be put on the threads that were just initialized with init_sensor_threads
        self.init_sensor_workers(mmw_interface, uwb_interface_anchor, uwb_interface_tag,
                                 leap_interface, xeThruX4Sensor_interface)

        # timer
        self.timer = QTimer()
        self.timer.setInterval(refresh_interval)
        self.timer.timeout.connect(self.ticks)
        self.timer.start()

        # Initialize tab screen

        self.tabs = QTabWidget()
        self.tab1 = ControlTab(mmw_worker=self.workers['mmw'],
                               uwb_worker=self.workers['uwb'],
                               leap_worker=self.workers['leap'],
                               Xe4Thru_worker=self.workers['xe4thru'],
                               refresh_interval=refresh_interval)

        self.tab2 = RadarTab(self.workers['mmw'], refresh_interval, data_path)
        self.tab3 = LeapTab(self.workers['leap'], refresh_interval, data_path)
        self.tab4 = UWBTab(self.workers['uwb'], refresh_interval, data_path)
        self.tab5 = XeThruX4Tab(self.workers['xe4thru'], refresh_interval, data_path)
        self.tab6 = GestureTab(self.workers['mmw'], self.workers['leap'], self.workers['xe4thru'])  # TODO add other sensors

        self.tabs.addTab(self.tab1, config.main_window_control_tab_label)
        self.tabs.addTab(self.tab2, config.main_window_radar_tab_label)
        self.tabs.addTab(self.tab3, config.main_window_leap_tab_label)
        self.tabs.addTab(self.tab4, config.main_window_uwb_tab_label)
        self.tabs.addTab(self.tab5, "XeThruX4")
        self.tabs.addTab(self.tab6, config.main_window_gesture_tab_label)

        # Add tabs to main_widget
        self.layout.addWidget(self.tabs)
        self.setLayout(self.layout)

        self.info_pane = InformationPane(parent=self.layout)
        sys.stdout = Stream(newText=self.on_print)

    def init_sensor_threads(self):
        self.worker_threads = {
            'mmw': pg.QtCore.QThread(self),
            'uwb': pg.QtCore.QThread(self),
            'leap': pg.QtCore.QThread(self),
            'xe4thru': pg.QtCore.QThread(self)
        }
        [w.start() for w in self.worker_threads.values()]  # start all the worker threads

    def init_sensor_workers(self, mmw_interface, uwb_interface_anchor, uwb_interface_tag,
                            leap_interface, xeThruX4Sensor_interface):
        self.workers = {
            'mmw': workers.MmwWorker(mmw_interface),
            'uwb': workers.UWBWorker(uwb_interface_anchor, uwb_interface_tag),
            'leap': workers.LeapWorker(leap_interface=leap_interface),
            'xe4thru': workers.Xe4ThruWorker(xeThruX4Sensor_interface)
        }
        self.workers['mmw'].moveToThread(self.worker_threads['mmw'])
        self.workers['uwb'].moveToThread(self.worker_threads['uwb'])
        self.workers['leap'].moveToThread(self.worker_threads['leap'])
        self.workers['xe4thru'].moveToThread(self.worker_threads['leap'])

    def on_print(self, msg):
        self.info_pane.push(msg)

    def __del__(self):
        sys.stdout = sys.__stdout__  # return control to regular stdout

    @pg.QtCore.pyqtSlot()
    def ticks(self):
        """
        ticks every 'refresh' milliseconds
        """
        [w.tick_signal.emit() for w in self.workers.values()]
