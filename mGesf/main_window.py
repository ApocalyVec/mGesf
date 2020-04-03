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

    def __init__(self, *args, **kwargs):
        super(mmw_worker, self).__init__()
        self.tick_signal.connect(self.mmw_process_on_tick)
        self._mmw_interface = None

    @pg.QtCore.pyqtSlot()
    def mmw_process_on_tick(self):
        start = time.time()
        if self._mmw_interface:
            try:
                pts_array, range_amplitude, rd_heatmap = self._mmw_interface.process_frame()
            except DataPortNotOpenError:  # happens when the emitted signal accumulates
                return
            if range_amplitude is None:  # replace with simulated data if not enabled
                range_amplitude = sim_imp()
            if rd_heatmap is None:
                rd_heatmap = sim_heatmap((16, 16))
        else:  # this is in simulation mode
            pts_array = sim_detected_points()
            range_amplitude = sim_imp()
            rd_heatmap = sim_heatmap((16, 16))
        # notify the mmw data frame is ready
        self.signal_mmw_frame_ready.emit({'range_doppler': rd_heatmap,
                                          'pts': pts_array,
                                          'range_amplitude': range_amplitude})

        self.timing_list.append(time.time() - start)  # TODO refactor timing calculation

    def enable_mmw(self, mmw_interface):
        self._mmw_interface = mmw_interface
        self._mmw_interface.start_sensor()

    def stop_sensors(self):
        self.stop_mmw()  # stop the mmWave sensor
        print('frame rate is ' + str(1 / np.mean(self.timing_list)))  # TODO refactor timing calculation

    def stop_mmw(self):
        if self._mmw_interface:
            self._mmw_interface.stop_sensor()


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
        self.interruptBtn = QtWidgets.QPushButton(text='Stop Sensor')
        self.interruptBtn.clicked.connect(self.interruptBtnAction)
        self.mmw_lay.addWidget(self.interruptBtn, *(0, 0))

        # add the start record button button
        self.is_record = False
        self.recordBtn = QtWidgets.QPushButton(text='Start Recording')
        self.recordBtn.clicked.connect(self.recordBtnAction)
        self.mmw_lay.addWidget(self.recordBtn, *(1, 0))

        # add dialogue label
        self.dialogueLabel = QLabel()
        self.dialogueLabel.setText("Running")
        self.mmw_lay.addWidget(self.dialogueLabel, *(2, 0))

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

        self.worker = mmw_worker()
        self.worker.moveToThread(self.worker_thread)
        # connect the mmWave frame signal to the function that processes the data
        self.worker.signal_mmw_frame_ready.connect(self.process_mmw_data)

        # prepare the sensor interface
        if mmw_interface:
            print('App: using IWR6843AoP; starting sensor')
            self.worker.enable_mmw(mmw_interface)
            print('App: done!')
        else:
            print('App: not using IWR6843AoP')

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
        # pts_gv = QGraphicsView()
        # self.lay.addWidget(pts_gv, *(0, 3))
        # scene = QGraphicsScene(self)
        # pts_gv.setScene(scene)
        # scene.addItem(self.pts_pixmap_item)

    def init_curve_view(self, pos, x_lim, y_lim):
        curve_plt = pg.PlotWidget()
        curve_plt.setXRange(*x_lim)
        curve_plt.setYRange(*y_lim)
        self.mmw_lay.addWidget(curve_plt, *pos)
        curve = curve_plt.plot([], [], pen=pg.mkPen(color=(0, 0, 255)))
        return curve

    def interruptBtnAction(self):
        self.interruptBtn.setDisabled(True)
        self.timer.stop()
        self.worker.stop_sensors()
        self.worker_thread.quit()
        self.dialogueLabel.setText('Stopped. Close the application to return to the console.')

    def recordBtnAction(self):
        if not self.is_record:
            self.is_record = True
            self.recordBtn.setText("Stop Recording")
        else:
            self.is_record = False
            self.recordBtn.setText("Start Recording")

            today = datetime.now()
            pickle.dump(self.buffer, open(os.path.join(self.data_path,
                                                       today.strftime("%b-%d-%Y-%H-%M-%S") + '.mgesf'), 'wb'))
            print('data save to ' + self.data_path)

        # self.recordBtn.setDisabled(True)
        # self.timer.stop()
        # self.worker.stop_sensors()
        # self.worker_thread.quit()
        # self.dialogueLabel.setText('Stopped. Close the application to return to the console.')

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
        self.worker.tick_signal.emit()  # signals the worker to run process_on_tick
