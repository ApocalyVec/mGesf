import os
import pickle
import time
from datetime import datetime

from PyQt5 import QtWidgets
from PyQt5.QtCore import pyqtSignal, QObject, QTimer
from PyQt5.QtGui import QPixmap
from PyQt5.QtWidgets import QGraphicsPixmapItem, QGraphicsScene, QGraphicsView, QWidget, \
    QGridLayout, QMainWindow, QLabel
import pyqtgraph as pg

from mGesf.exceptions import DataPortNotOpenError
from utils.iwr6843_utils.mmWave_interface import MmWaveSensorInterface
from utils.simulation import sim_heatmap, sim_detected_points, sim_imp
from utils.img_utils import array_to_colormap_qim

import numpy as np


# class WorkerSignals(QObject):
#     finished = pyqtSignal()
#     error = pyqtSignal(tuple)
#     result = pyqtSignal(object)
#     progress = pyqtSignal(int)

class mmw_worker(QObject):
    signal_mmw_frame_ready = pyqtSignal(dict)
    tick_signal = pyqtSignal()
    timing_list = []  # TODO refactor timing calculation

    def __init__(self, mmw_interface=None, *args, **kwargs):
        super(mmw_worker, self).__init__()
        self.tick_signal.connect(self.mmw_process_on_tick)
        self._mmw_interface = mmw_interface
        self._is_running = False

    @pg.QtCore.pyqtSlot()
    def mmw_process_on_tick(self):
        if self._is_running:
            if self._mmw_interface:
                try:
                    start = time.time()
                    pts_array, range_amplitude, rd_heatmap = self._mmw_interface.process_frame()
                except DataPortNotOpenError:  # happens when the emitted signal accumulates
                    return
                if range_amplitude is None:  # replace with simulated data if not enabled
                    range_amplitude = sim_imp()
                if rd_heatmap is None:
                    rd_heatmap = sim_heatmap((16, 16))
                self.timing_list.append(time.time() - start)  # TODO refactor timing calculation

            else:  # this is in simulation mode
                pts_array = sim_detected_points()
                range_amplitude = sim_imp()
                rd_heatmap = sim_heatmap((16, 16))
            # notify the mmw data frame is ready
            self.signal_mmw_frame_ready.emit({'range_doppler': rd_heatmap,
                                              'pts': pts_array,
                                              'range_amplitude': range_amplitude})

    def start_mmw(self):
        self._mmw_interface.start_sensor()
        self._is_running = True

    def stop_mmw(self):
        self._is_running = False
        time.sleep(0.1)  # wait 100ms for the previous frames to finish process
        if self._mmw_interface:
            self._mmw_interface.stop_sensor()
        if self._mmw_interface:
            print('frame rate is ' + str(1 / np.mean(self.timing_list)))  # TODO refactor timing calculation
        else:
            print('frame rate calculation is not enabled in simulation mode')

    def is_connected(self):
        return self._mmw_interface.is_connected()

    def connect_mmw(self, uport_name, dport_name):
        if self._mmw_interface:
            self._mmw_interface.connect(uport_name, dport_name)

    def disconnect_mmw(self):
        self.stop_mmw()
        if self._mmw_interface:
            self._mmw_interface.close_connection()

    def send_config(self, config_path):
        if self._mmw_interface:
            self._mmw_interface.send_config(config_path)
            self.start_mmw()

    def is_mmw_running(self):
        return self._is_running


# TODO add resume function to the stop button
class MainWindow(QMainWindow):
    def __init__(self, mmw_interface: MmWaveSensorInterface, refresh_interval, data_path, *args, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)
        self.resize(1280, 720)
        pg.setConfigOption('background', 'w')

        w = QWidget()
        # create mGesf layout
        self.mmw_lay = QGridLayout(self)
        # add spectrogram graphic view
        self.spec_pixmap_item = QGraphicsPixmapItem()
        self.init_spec_view(pos=(0, 2))
        # add detected points plots
        self.scatterXY = self.init_pts_view(pos=(0, 3), x_lim=(-0.5, 0.5), y_lim=(0, 1.))
        self.scatterZD = self.init_pts_view(pos=(1, 2), x_lim=(-0.5, 0.5), y_lim=(-1., 1.))
        self.ra_view = self.init_curve_view(pos=(1, 3), x_lim=(-10, 260), y_lim=(1500, 3800))

        # add the interrupt button
        self.start_stop_btn = QtWidgets.QPushButton(text='Stop Sensor')
        self.start_stop_btn.clicked.connect(self.start_stop_btn_action)
        self.mmw_lay.addWidget(self.start_stop_btn, *(0, 0))

        # add the start record button
        self.is_record = False
        self.record_btn = QtWidgets.QPushButton(text='Start Recording')
        self.record_btn.clicked.connect(self.record_btn_action)
        self.mmw_lay.addWidget(self.record_btn, *(1, 0))

        # com port entries
        # Create textbox
        self.uport_textbox = QtWidgets.QLineEdit()
        self.uport_textbox.setPlaceholderText('User Port')
        self.mmw_lay.addWidget(self.uport_textbox, *(2, 0))

        # Create textbox
        self.dport_textbox = QtWidgets.QLineEdit()
        self.dport_textbox.setPlaceholderText('Data Port')
        self.mmw_lay.addWidget(self.dport_textbox, *(3, 0))

        # add close connection button
        self.connection_btn = QtWidgets.QPushButton(text='Connect')
        self.connection_btn.clicked.connect(self.connection_btn_action)
        self.mmw_lay.addWidget(self.connection_btn, *(4, 0))

        # Create textbox
        self.config_textbox = QtWidgets.QLineEdit()
        self.config_textbox.setPlaceholderText('Config File Path')
        self.mmw_lay.addWidget(self.config_textbox, *(5, 0))

        # add send config button
        self.config_btn = QtWidgets.QPushButton(text='Send Config')
        self.config_btn.clicked.connect(self.config_btn_action)
        self.mmw_lay.addWidget(self.config_btn, *(6, 0))

        self.dialogueLabel = QLabel()
        self.dialogueLabel.setText("Running")
        self.mmw_lay.addWidget(self.dialogueLabel, *(7, 0))

        # set the mGesf layout
        w.setLayout(self.mmw_lay)
        self.setCentralWidget(w)
        self.show()

        # create the data buffers
        self.buffer = {'mmw': {'timestamps': [], 'ra_profile': [], 'rd_heatmap': [], 'detected_points': []}}
        self.data_path = data_path

        # create threading
        # create a QThread and start the thread that handles
        self.worker_thread = pg.QtCore.QThread(self)
        self.worker_thread.start()

        self.timer = QTimer()
        self.timer.setInterval(refresh_interval)
        self.timer.timeout.connect(self.ticks)

        self.mmw_worker = mmw_worker(mmw_interface)
        self.mmw_worker.moveToThread(self.worker_thread)
        # connect the mmWave frame signal to the function that processes the data
        self.mmw_worker.signal_mmw_frame_ready.connect(self.process_mmw_data)

        # prepare the sensor interface
        # if mmw_interface:
        #     print('App: using IWR6843AoP; starting sensor')
        #     self.mmw_worker.start_mmw()
        #     print('App: done!')
        # else:
        #     print('App: not using IWR6843AoP')

        self.timer.start()

    def init_spec_view(self, pos):
        spc_gv = QGraphicsView()
        self.mmw_lay.addWidget(spc_gv, *pos)
        scene = QGraphicsScene(self)
        spc_gv.setScene(scene)
        scene.addItem(self.spec_pixmap_item)

    def init_pts_view(self, pos, x_lim, y_lim):
        pts_plt = pg.PlotWidget()
        pts_plt.setXRange(*x_lim)
        pts_plt.setYRange(*y_lim)
        self.mmw_lay.addWidget(pts_plt, *pos)
        scatter = pg.ScatterPlotItem(pen=None, symbol='o')
        pts_plt.addItem(scatter)
        return scatter

    def init_curve_view(self, pos, x_lim, y_lim):
        curve_plt = pg.PlotWidget()
        curve_plt.setXRange(*x_lim)
        curve_plt.setYRange(*y_lim)
        self.mmw_lay.addWidget(curve_plt, *pos)
        curve = curve_plt.plot([], [], pen=pg.mkPen(color=(0, 0, 255)))
        return curve

    def config_btn_action(self):
        # TODO add check if file exits
        self.mmw_worker.send_config(self.config_textbox.text())

    def start_stop_btn_action(self):
        if self.mmw_worker.is_mmw_running():
            self.start_stop_btn.setText('Start Sensor')
            self.mmw_worker.stop_mmw()
            self.dialogueLabel.setText('Stopped.')
        else:
            self.start_stop_btn.setText('Stop Sensor')
            self.mmw_worker.start_mmw()
            self.dialogueLabel.setText('Running.')

    def record_btn_action(self):
        if not self.is_record:
            self.is_record = True
            self.record_btn.setText("Stop Recording")
        else:
            self.is_record = False
            self.record_btn.setText("Start Recording")

            today = datetime.now()
            pickle.dump(self.buffer, open(os.path.join(self.data_path,
                                                       today.strftime("%b-%d-%Y-%H-%M-%S") + '.mgesf'), 'wb'))
            print('data save to ' + self.data_path)

    def connection_btn_action(self):
        if self.mmw_worker.is_connected():
            self.mmw_worker.disconnect_mmw()
            self.connection_btn.setText('Connect')
        else:
            self.mmw_worker.connect_mmw(uport_name=self.uport_textbox.text(), dport_name=self.dport_textbox.text())
            self.connection_btn.setText('Disconnect')

    def config_btn_action(self):
        self.mmw_worker.send_config(config_path=self.config_textbox.text())

    def process_mmw_data(self, data_dict):
        """
        Process the emitted mmWave data
        This function is evoked when signaled by self.mmw_data_ready which is emitted by the mmw_worker thread.
        The function handles the following actions
            update the mmw figures in the GUI
            record the mmw data if record is enabled. In the current implementation, the data is provisionally saved in
            the memory and evicted when the user click 'stop_record'
        :param data_dict:
        """
        # update spectrogram
        rdh_qim = array_to_colormap_qim(data_dict['range_doppler'])
        spec_qpixmap = QPixmap(rdh_qim)
        # spec_qpixmap = spec_qpixmap.scaled(256, 512)  # resize spectrogram
        spec_qpixmap = spec_qpixmap.scaled(512, 512, pg.QtCore.Qt.KeepAspectRatio)  # resize spectrogram
        self.spec_pixmap_item.setPixmap(spec_qpixmap)
        # update the 2d scatter plot for the detected points
        self.scatterXY.setData(data_dict['pts'][:, 0], data_dict['pts'][:, 1])
        self.scatterZD.setData(data_dict['pts'][:, 2], data_dict['pts'][:, 3])

        # update range amplitude profile
        ra = np.asarray(data_dict['range_amplitude'])
        range_bin_space = np.asarray(range(len(ra)))
        self.ra_view.setData(range_bin_space, ra)

        # save the data is record is enabled
        # mmw buffer: {'timestamps': [], 'ra_profile': [], 'rd_heatmap': [], 'detected_points': []}
        if self.is_record:
            self.buffer['mmw']['timestamps'].append(time.time())
            self.buffer['mmw']['ra_profile'].append(data_dict['range_doppler'])
            self.buffer['mmw']['rd_heatmap'].append(ra)
            self.buffer['mmw']['detected_points'].append(data_dict['pts'])

    @pg.QtCore.pyqtSlot()
    def ticks(self):
        """
        ticks every 'refresh' milliseconds
        """
        self.mmw_worker.tick_signal.emit()  # signals the worker to run process_on_tick
