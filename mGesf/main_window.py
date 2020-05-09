import os
import pickle
import time
from datetime import datetime

from PyQt5 import QtWidgets
from PyQt5.QtCore import QTimer, pyqtSlot
from PyQt5.QtGui import QPixmap
from PyQt5.QtWidgets import QGraphicsPixmapItem, QWidget, QMainWindow, QLabel, QVBoxLayout, QPushButton, QTabWidget
import pyqtgraph as pg

from utils.iwr6843_utils.mmWave_interface import MmWaveSensorInterface
from utils.img_utils import array_to_colormap_qim

import numpy as np
import mGesf.MMW_worker as MMW_worker

# tabs ======================================
import mGesf.control_tab as control_tab
import mGesf.radar_tab as radar_tab
import mGesf.leap_tab as leap_tab
import mGesf.UWB_tab as UWB_tab
import mGesf.config as config


# class WorkerSignals(QObject):
#     finished = pyqtSignal()
#     error = pyqtSignal(tuple)
#     result = pyqtSignal(object)
#     progress = pyqtSignal(int)

# TODO add resume function to the stop button
class MainWindow(QMainWindow):
    def __init__(self, mmw_interface: MmWaveSensorInterface, refresh_interval, data_path, *args, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)
        self.resize(config.WINDOW_WIDTH, config.WINDOW_HEIGHT)
        pg.setConfigOption('background', 'w')

        # create the tabs
        self.table_widget = Tabs(self, mmw_interface, refresh_interval, data_path)
        self.setCentralWidget(self.table_widget)
        self.show()


class Tabs(QWidget):
    """A frame contains 4 tabs and their contents"""

    def __init__(self, parent, mmw_interface: MmWaveSensorInterface, refresh_interval, data_path, *args, **kwargs):
        super(QWidget, self).__init__(parent)
        self.layout = QVBoxLayout(self)

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

        self.tabs.addTab(self.tab1, config.tab1_label)
        self.tabs.addTab(self.tab2, config.tab2_label)
        self.tabs.addTab(self.tab3, config.tab3_label)
        self.tabs.addTab(self.tab4, config.tab4_label)

        # Add tabs to widget
        self.layout.addWidget(self.tabs)
        self.setLayout(self.layout)

    @pg.QtCore.pyqtSlot()
    def ticks(self):
        """
        ticks every 'refresh' milliseconds
        """
        self.mmw_worker.tick_signal.emit()  # signals the worker to run process_on_tick
