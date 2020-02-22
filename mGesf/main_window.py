import time

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
range_bins = 256
range_bin_space = np.asarray(range(range_bins))


class Worker(QObject):
    result_signal = pyqtSignal(dict)
    tick_signal = pyqtSignal()
    timing_list = []  # TODO refactor timing calculation

    def __init__(self, *args, **kwargs):
        super(Worker, self).__init__()
        self.tick_signal.connect(self.mmw_process_on_tick)
        self._mmw_interface = None

    @pg.QtCore.pyqtSlot()
    def mmw_process_on_tick(self):
        start = time.time()
        if self._mmw_interface:
            try:
                pts_array, range_profile, rd_heatmap = self._mmw_interface.process_frame()
                if rd_heatmap is not None:
                    rd_heatmap = rd_heatmap[:, :32]  # TODO don't hard code this value!
            except DataPortNotOpenError:  # happens when the emitted signal accumulates
                return
            if range_profile is None:  # replace with simulated data if not enabled
                range_profile = sim_imp()
            if rd_heatmap is None:
                rd_heatmap = sim_heatmap((16, 32))

            rdh_qim = array_to_colormap_qim(rd_heatmap)
            self.result_signal.emit({'spec': rdh_qim,
                                     'pts': pts_array,
                                     'imp': range_profile})
        else:  # this is in simulation mode
            pts_array = sim_detected_points()
            range_profile = sim_imp()
            rd_heatmap = sim_heatmap((16, 32))
            rdh_qim = array_to_colormap_qim(rd_heatmap)
            self.result_signal.emit({'spec': rdh_qim,
                                     'pts': pts_array,
                                     'imp': range_profile})

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
    def __init__(self, mmw_interface: MmWaveSensorInterface, refresh_interval, *args, **kwargs):
        super(MainWindow, self).__init__(*args, **kwargs)
        self.resize(1280, 720)
        pg.setConfigOption('background', 'w')

        w = QWidget()
        # create mGesf layout
        self.lay = QGridLayout(self)
        # add spectrogram graphic view
        self.spec_pixmap_item = QGraphicsPixmapItem()
        self.init_spec_view(pos=(0, 2))
        # add detected points plots
        self.scatterXY = self.init_pts_view(pos=(0, 3), x_lim=(-0.5, 0.5), y_lim=(0, 1.))
        self.scatterZD = self.init_pts_view(pos=(1, 2), x_lim=(-0.5, 0.5), y_lim=(-1., 1.))
        self.curveImp = self.init_curve_view(pos=(1, 3), x_lim=(-10, 260), y_lim=(1500, 3800))

        # add the interrupt button
        self.interruptBtn = QtWidgets.QPushButton(text='Stop')
        self.interruptBtn.clicked.connect(self.interruptBtnAction)
        self.lay.addWidget(self.interruptBtn, *(0, 1))

        # add dialogue label
        self.dialogueLabel = QLabel()
        self.dialogueLabel.setText("Running")
        self.lay.addWidget(self.dialogueLabel, *(0, 0))

        # set the mGesf layout
        w.setLayout(self.lay)
        self.setCentralWidget(w)
        self.show()

        # create threading
        # create a QThread and start the thread that handles
        self.worker_thread = pg.QtCore.QThread(self)
        self.worker_thread.start()

        self.timer = QTimer()
        self.timer.setInterval(refresh_interval)
        self.timer.timeout.connect(self.ticks)

        self.worker = Worker()
        self.worker.moveToThread(self.worker_thread)
        self.worker.result_signal.connect(self.update_image)

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
        self.lay.addWidget(spc_gv, *pos)
        scene = QGraphicsScene(self)
        spc_gv.setScene(scene)
        scene.addItem(self.spec_pixmap_item)

    def init_pts_view(self, pos, x_lim, y_lim):
        pts_plt = pg.PlotWidget()
        pts_plt.setXRange(* x_lim)
        pts_plt.setYRange(* y_lim)
        self.lay.addWidget(pts_plt, *pos)
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
        curve_plt.setXRange(* x_lim)
        curve_plt.setYRange(* y_lim)
        self.lay.addWidget(curve_plt, *pos)
        curve = curve_plt.plot([], [], pen=pg.mkPen(color=(0, 0, 255)))
        return curve

    def interruptBtnAction(self):
        self.interruptBtn.setDisabled(True)
        self.timer.stop()
        self.worker.stop_sensors()
        self.worker_thread.quit()
        self.dialogueLabel.setText('Stopped. Close the application to return to the console.')

    def update_image(self, data_dict):
        # update spectrogram
        spec_qpixmap = QPixmap(data_dict['spec'])
        spec_qpixmap = spec_qpixmap.scaled(256, 512)  # resize spectrogram
        # spec_qpixmap = spec_qpixmap.scaled(512, 512, pg.QtCore.Qt.KeepAspectRatio)  # resize spectrogram
        self.spec_pixmap_item.setPixmap(spec_qpixmap)
        # update the scatter
        self.scatterXY.setData(data_dict['pts'][:, 0], data_dict['pts'][:, 1])
        self.scatterZD.setData(data_dict['pts'][:, 2], data_dict['pts'][:, 3])
        self.curveImp.setData(range_bin_space, np.asarray(data_dict['imp']))

    @pg.QtCore.pyqtSlot()
    def ticks(self):
        """
        ticks every 'refresh' milliseconds
        """
        self.worker.tick_signal.emit()  # signals the worker to run process_on_tick
