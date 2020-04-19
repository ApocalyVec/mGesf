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
                    pts_array, range_amplitude, rd_heatmap, azi_heatmap = self._mmw_interface.process_frame()
                except DataPortNotOpenError:  # happens when the emitted signal accumulates
                    return
                if range_amplitude is None:  # replace with simulated data if not enabled
                    range_amplitude = sim_imp()
                if rd_heatmap is None:
                    rd_heatmap = sim_heatmap((8, 16))
                if azi_heatmap is None:
                    azi_heatmap = sim_heatmap(8, 24)
                self.timing_list.append(time.time() - start)  # TODO refactor timing calculation

            else:  # this is in simulation mode
                pts_array = sim_detected_points()
                range_amplitude = sim_imp()
                rd_heatmap = sim_heatmap((8, 16))
                azi_heatmap = sim_heatmap(8, 24)

            # notify the mmw data frame is ready
            self.signal_mmw_frame_ready.emit({'range_doppler': rd_heatmap,
                                              'range_azi': azi_heatmap,
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

        # create mGesf layout ##########################################################################################
        window = QWidget()

        # create mmWave layout #####################################################
        main_hl = QtWidgets.QHBoxLayout(self)

        self.control_vl = QtWidgets.QVBoxLayout()  # create vbox for controls
        self.figure_gl = QtWidgets.QGridLayout()  # create grid layout for the figures

        main_hl.addLayout(self.control_vl)
        main_hl.addLayout(self.figure_gl)

        # add the figures ##################################
        # add statistics
        statistics_vl = QtWidgets.QVBoxLayout()
        self.statistics_ui = {'pid': QLabel(),
                              'ver': QLabel(),
                              'dlen': QLabel(),
                              'numTLVs': QLabel(),
                              'numObj': QLabel(),
                              'pf': QLabel()}
        [v.setText(k) for k, v in self.statistics_ui.items()]
        [statistics_vl.addWidget(v) for v in self.statistics_ui.values()]
        self.figure_gl.addLayout(statistics_vl, *(0, 0))  # why does not this show up

        # print("Packet ID:\t%d "%(frameNum))
        # print("Version:\t%x "%(version))
        # print("Data Len:\t\t%d", length)
        # print("TLV:\t\t%d "%(numTLVs))
        # print("Detect Obj:\t%d "%(numObj))
        # print("Platform:\t%X "%(platform))
        # add range doppler
        self.doppler_display = QGraphicsPixmapItem()
        self.init_spec_view(pos=(1, 1), label='Range Doppler Profile')

        # add range azi
        self.azi_display = QGraphicsPixmapItem()
        self.init_spec_view(pos=(1, 2), label='Range Azimuth Profile')

        # add detected points plots
        self.scatterXY = self.init_pts_view(pos=(0, 1), label='Detected Points XY', x_lim=(-0.5, 0.5), y_lim=(0, 1.))
        self.scatterZD = self.init_pts_view(pos=(0, 2), label='Detected Points ZD',  x_lim=(-0.5, 0.5), y_lim=(-1., 1.))
        self.ra_view = self.init_curve_view(pos=(1, 0), label='Range Profile',  x_lim=(-10, 260), y_lim=(1500, 3800))

        # add the controls ##################################
        # add the interrupt button
        self.start_stop_btn = QtWidgets.QPushButton(text='Stop Sensor')
        self.start_stop_btn.clicked.connect(self.start_stop_btn_action)
        self.control_vl.addWidget(self.start_stop_btn)

        # add the start record button
        self.is_record = False
        self.record_btn = QtWidgets.QPushButton(text='Start Recording')
        self.record_btn.clicked.connect(self.record_btn_action)
        self.control_vl.addWidget(self.record_btn)

        # com port entries
        # Create textbox
        self.uport_textbox = QtWidgets.QLineEdit()
        self.uport_textbox.setPlaceholderText('User Port')
        self.control_vl.addWidget(self.uport_textbox)

        # Create textbox
        self.dport_textbox = QtWidgets.QLineEdit()
        self.dport_textbox.setPlaceholderText('Data Port')
        self.control_vl.addWidget(self.dport_textbox)

        # add close connection button
        self.connection_btn = QtWidgets.QPushButton(text='Connect')
        self.connection_btn.clicked.connect(self.connection_btn_action)
        self.control_vl.addWidget(self.connection_btn)

        # Create textbox
        self.config_textbox = QtWidgets.QLineEdit()
        self.config_textbox.setPlaceholderText('Config File Path')
        self.control_vl.addWidget(self.config_textbox)

        # add send config button
        self.config_btn = QtWidgets.QPushButton(text='Send Config')
        self.config_btn.clicked.connect(self.config_btn_action)
        self.control_vl.addWidget(self.config_btn)

        self.dialogueLabel = QLabel()
        self.dialogueLabel.setText("Running")
        self.control_vl.addWidget(self.dialogueLabel)

        # set the mGesf layout
        window.setLayout(main_hl)
        self.setCentralWidget(window)
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

    def init_spec_view(self, pos, label):
        vl = QtWidgets.QVBoxLayout()

        ql = QLabel()
        ql.setText(label)
        vl.addWidget(ql)

        spc_gv = QGraphicsView()
        vl.addWidget(spc_gv)

        self.figure_gl.addLayout(vl, *pos)
        scene = QGraphicsScene(self)
        spc_gv.setScene(scene)
        scene.addItem(self.doppler_display)

    def init_pts_view(self, pos, label, x_lim, y_lim):
        vl = QtWidgets.QVBoxLayout()

        ql = QLabel()
        ql.setText(label)
        vl.addWidget(ql)

        pts_plt = pg.PlotWidget()
        vl.addWidget(pts_plt)
        self.figure_gl.addLayout(vl, *pos)

        pts_plt.setXRange(*x_lim)
        pts_plt.setYRange(*y_lim)
        scatter = pg.ScatterPlotItem(pen=None, symbol='o')
        pts_plt.addItem(scatter)
        return scatter

    def init_curve_view(self, pos, label, x_lim, y_lim):
        vl = QtWidgets.QVBoxLayout()

        ql = QLabel()
        ql.setText(label)
        vl.addWidget(ql)

        curve_plt = pg.PlotWidget()
        vl.addWidget(curve_plt)
        self.figure_gl.addLayout(vl, *pos)

        curve_plt.setXRange(*x_lim)
        curve_plt.setYRange(*y_lim)
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
        # update range doppler spectrogram
        doppler_heatmap_qim = array_to_colormap_qim(data_dict['range_doppler'])
        doppler_qpixmap = QPixmap(doppler_heatmap_qim)
        doppler_qpixmap = doppler_qpixmap.scaled(512, 512, pg.QtCore.Qt.KeepAspectRatio)  # resize spectrogram
        self.doppler_display.setPixmap(doppler_qpixmap)

        # update range azimuth spectrogram
        azi_heatmap_qim = array_to_colormap_qim(data_dict['range_doppler'])
        azi_qpixmap = QPixmap(azi_heatmap_qim)
        azi_qpixmap = azi_qpixmap.scaled(512, 512, pg.QtCore.Qt.KeepAspectRatio)  # resize spectrogram
        self.azi_display.setPixmap(azi_qpixmap)

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
