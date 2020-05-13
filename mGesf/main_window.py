from PyQt5.QtCore import QTimer
from PyQt5.QtWidgets import QWidget, QMainWindow, QVBoxLayout, QTabWidget, \
    QHBoxLayout
import pyqtgraph as pg

from utils.GUI_main_window import init_information_block
from utils.iwr6843_utils.mmWave_interface import MmWaveSensorInterface

import mGesf.MMW_worker as MMW_worker

# tabs ======================================
import mGesf.main_page_tabs.control_tab as control_tab
import mGesf.main_page_tabs.radar_tab as radar_tab
import mGesf.main_page_tabs.leap_tab as leap_tab
import mGesf.main_page_tabs.UWB_tab as UWB_tab
import mGesf.main_page_tabs.gesture_tab.gesture_tab as gesture_tab
import config as config




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

        layout = QVBoxLayout()

        # create the tabs and information black
        self.table_widget = Tabs(self, mmw_interface, refresh_interval, data_path)
        self.setCentralWidget(self.table_widget)

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
        self.tab5 = gesture_tab.Gesture_tab()

        self.tabs.addTab(self.tab1, config.main_window_control_tab_label)
        self.tabs.addTab(self.tab2, config.main_window_radar_tab_label)
        self.tabs.addTab(self.tab3, config.main_window_leap_tab_label)
        self.tabs.addTab(self.tab4, config.main_window_uwb_tab_label)
        self.tabs.addTab(self.tab5, config.main_window_gesture_tab_label)

        # Add tabs to widget
        self.layout.addWidget(self.tabs)
        self.setLayout(self.layout)

        # ***** information block *****
        self.scrollArea, self.message = init_information_block(parent=self.layout)

    @pg.QtCore.pyqtSlot()
    def ticks(self):
        """
        ticks every 'refresh' milliseconds
        """
        self.mmw_worker.tick_signal.emit()  # signals the worker to run process_on_tick
