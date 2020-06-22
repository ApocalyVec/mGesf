import os
import pickle
import time
from datetime import datetime

from PyQt5 import QtWidgets
from PyQt5.QtCore import pyqtSlot, QTimer
from PyQt5.QtGui import QPixmap
from PyQt5.QtWidgets import QGraphicsPixmapItem, QWidget, QMainWindow, QLabel, QVBoxLayout, QPushButton, QTabWidget, \
    QGraphicsView, QGraphicsScene
import pyqtgraph as pg
from PyQt5 import QtCore

from utils.img_utils import array_to_colormap_qim

import numpy as np
import mGesf.workers as workers
from utils.iwr6843_utils.mmWave_interface import MmWaveSensorInterface


def init_view(label):
    vl = QtWidgets.QVBoxLayout()
    ql = QLabel()
    ql.setAlignment(QtCore.Qt.AlignCenter)
    ql.setText(label)
    vl.addWidget(ql)
    return vl


class UWB_tab(QWidget):
    def __init__(self, uwb_worker: workers.UWBWorker, refresh_interval, *args, **kwargs):
        super().__init__()

        self.uwb_worker = uwb_worker

        main_page = QtWidgets.QVBoxLayout(self)
        self.setLayout(main_page)
        self.figure_gl = QtWidgets.QGridLayout()  # create grid layout for the figures
        self.info_vl = QtWidgets.QVBoxLayout()  # create vbox for controls
        main_page.addLayout(self.figure_gl)
        main_page.addLayout(self.info_vl)

        # ====================== Add graphs to the grid =======================================

        #  ----------- Anchor Impluse response -------------------------
        self.pen1 = pg.mkPen(color=(255, 0, 0), width=2)
        self.pen2 = pg.mkPen(color=(0, 255, 0), width=2)
        self.pen3 = pg.mkPen(color=(0, 255, 255), width=2)

        #  ----------- Anchor Impluse response -------------------------

        self.ARI_real, self.ARI_imag = self.init_real_imag_curve_view(pos=(0, 0),
                                                                      label='Anchor Impluse response(Real, imag)',
                                                                      x_lim=(0, 65),
                                                                      y_lim=(-25000, 25000))

        self.AM = self.init_mag_curve_view(pos=(0, 1),
                                           label='Anchor Impluse response(mag)',
                                           x_lim=(0, 65),
                                           y_lim=(0, 25000))

        self.AP = self.init_mag_curve_view(pos=(0, 2),
                                           label='Anchor Impluse response(phase)',
                                           x_lim=(0, 65),
                                           y_lim=(0, 25000))

        #  ----------- Tag Impluse response -------------------------
        self.TRI_real, self.TRI_imag = self.init_real_imag_curve_view(pos=(1, 0),
                                                                      label='Tag Impluse response(Real, imag)',
                                                                      x_lim=(0, 65),
                                                                      y_lim=(-25000, 25000))

        self.TM = self.init_mag_curve_view(pos=(1, 1),
                                           label='Tag Impluse response(mag)',
                                           x_lim=(0, 65),
                                           y_lim=(0, 25000))

        self.TP = self.init_mag_curve_view(pos=(1, 2),
                                           label='Tag Impluse response(phase)',
                                           x_lim=(0, 65),
                                           y_lim=(0, 25000))

        # ====================== Add info to info_vl =======================================
        self.info_label = QLabel()
        self.info_label.setText("Information: ")
        self.info_vl.addWidget(self.info_label)

        # connect the mmWave frame signal to the function that processes the data
        self.uwb_worker.signal_data.connect(self.uwb_process_data)

        # prepare the sensor interface
        # if mmw_interface:
        #     print('App: using IWR6843AoP; starting sensor')
        #     self.mmw_worker.start_mmw()
        #     print('App: done!')
        # else:
        #     print('App: not using IWR6843AoP')

        self.show()

    def init_real_imag_curve_view(self, pos, label, x_lim, y_lim):
        vl = init_view(label)

        curve_plt = pg.PlotWidget()
        vl.addWidget(curve_plt)
        self.figure_gl.addLayout(vl, *pos)

        curve_plt.setXRange(*x_lim)
        curve_plt.setYRange(*y_lim)

        real_curve = curve_plt.plot([], [], pen=self.pen1, name="Real")
        imag_curve = curve_plt.plot([], [], pen=self.pen2, name="Imaginary")

        return real_curve, imag_curve

    def init_mag_curve_view(self, pos, label, x_lim, y_lim):
        vl = init_view(label)

        curve_plt = pg.PlotWidget()
        vl.addWidget(curve_plt)
        self.figure_gl.addLayout(vl, *pos)

        curve_plt.setXRange(*x_lim)
        curve_plt.setYRange(*y_lim)

        mag_curve = curve_plt.plot([], [], pen=self.pen3, name="Magnitude")
        return mag_curve

    def uwb_process_data(self, data_dict):
        """
        Process the emitted mmWave data
        This function is evoked when signaled by self.mmw_data_ready which is emitted by the mmw_worker thread.
        The function handles the following actions
            update the mmw figures in the GUI
            record the mmw data if record is enabled. In the current implementation, the data is provisionally saved in
            the memory and evicted when the user click 'stop_record'
        :param data_dict:
        """

        sensor_a = data_dict['a_frame']
        sensor_t = data_dict['t_frame']
        # print('UWB tab received data')

        # resolve anchor frame
        if data_dict['a_frame'] is not None:
            x_samples = list(range(data_dict['a_frame'].shape[0]))

            # real and imag data
            a_real = data_dict['a_frame'][:, 0]
            a_imag = data_dict['a_frame'][:, 1]

            # magnitude for anchor
            # a_mag = np.log10(np.sqrt(np.add(np.square(a_real), np.square(a_imag))))
            a_mag = np.sqrt(np.add(np.square(a_real), np.square(a_imag)))

            # plot real and imag and mag
            self.ARI_real.setData(x_samples, a_real, )
            self.ARI_imag.setData(x_samples, a_imag, )
            self.AM.setData(x_samples, a_mag)

        # resolve tag frame
        if data_dict['t_frame'] is not None:
            x_samples = list(range(data_dict['t_frame'].shape[0]))

            # real and imag data. anchor
            t_real = data_dict['t_frame'][:, 0]
            t_imag = data_dict['t_frame'][:, 1]

            # magnitude. tag
            # t_mag = np.log10(np.sqrt(np.add(np.square(t_real), np.square(t_imag))))
            t_mag = np.sqrt(np.add(np.square(t_real), np.square(t_imag)))

            # plot real and imag pairs
            self.TRI_real.setData(x_samples, t_real, )
            self.TRI_imag.setData(x_samples, t_imag, )
            self.TM.setData(x_samples, t_mag)
            # self.TM.addLegend()

        '''# update range doppler spectrogram
        doppler_heatmap_qim = array_to_colormap_qim(data_dict['range_doppler'])
        doppler_qpixmap = QPixmap(doppler_heatmap_qim)
        doppler_qpixmap = doppler_qpixmap.scaled(512, 512, pg.QtCore.Qt.KeepAspectRatio)  # resize spectrogram
        self.doppler_display.setPixmap(doppler_qpixmap)

        # update range azimuth spectrogram
        azi_heatmap_qim = array_to_colormap_qim(data_dict['range_azi'])
        azi_qpixmap = QPixmap(azi_heatmap_qim)
        azi_qpixmap = azi_qpixmap.scaled(512, 512, pg.QtCore.Qt.KeepAspectRatio)  # resize spectrogram
        self.azi_display.setPixmap(azi_qpixmap)

        # update the 2d scatter plot for the detected points
        self.scatterXY.setData(data_dict['pts'][:, 0], data_dict['pts'][:, 1])
        self.scatterZD.setData(data_dict['pts'][:, 2], data_dict['pts'][:, 3])

        # update range amplitude profile
        ra = np.asarray(data_dict['range_amplitude'])
        range_bin_space = np.asarray(range(len(ra)))
        self.ra_view.setData(range_bin_space, ra)'''

        # save the data is record is enabled
        # mmw buffer: {'timestamps': [], 'ra_profile': [], 'rd_heatmap': [], 'detected_points': []}
