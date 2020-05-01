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
import mGesf.MMW_worker as MMW_worker
from utils.iwr6843_utils.mmWave_interface import MmWaveSensorInterface


def init_view(label):
    vl = QtWidgets.QVBoxLayout()
    ql = QLabel()
    ql.setAlignment(QtCore.Qt.AlignCenter)
    ql.setText(label)
    vl.addWidget(ql)
    return vl


class Radar_tab(QWidget):
    def __init__(self, mmw_worker: MMW_worker, refresh_interval, *args, **kwargs):
        super().__init__()

        self.mmw_worker = mmw_worker

        main_page = QtWidgets.QVBoxLayout(self)
        self.setLayout(main_page)
        self.figure_gl = QtWidgets.QGridLayout()  # create grid layout for the figures
        self.info_vl = QtWidgets.QVBoxLayout()  # create vbox for controls
        main_page.addLayout(self.figure_gl)
        main_page.addLayout(self.info_vl)

        # ====================== Add graphs to the grid =======================================
        # add statistics
        self.statistics_view= self.init_statistics(pos=(0, 0), label='statistics')
        # add detected points plots
        self.scatterXY = self.init_pts_view(pos=(0, 1), label='Detected Points XY', x_lim=(-0.5, 0.5),
                                            y_lim=(0, 1.))
        self.scatterZD = self.init_pts_view(pos=(0, 2), label='Detected Points ZD', x_lim=(-0.5, 0.5),
                                            y_lim=(-1., 1.))
        self.ra_view = self.init_curve_view(pos=(1, 0), label='Range Profile', x_lim=(-10, 260),
                                            y_lim=(1500, 3800))
        # add range doppler
        self.doppler_display = self.init_spec_view(pos=(1, 1), label='Range Doppler Profile')
        # add range azi
        self.azi_display = self.init_spec_view(pos=(1, 2), label='Range Azimuth Profile')

        # ====================== Add info to info_vl =======================================
        self.info_label = QLabel()
        self.info_label.setText("Information: ")
        self.info_vl.addWidget(self.info_label)

        # connect the mmWave frame signal to the function that processes the data
        self.mmw_worker.signal_mmw_radar_tab.connect(self.radar_process_mmw_data)

        # prepare the sensor interface
        # if mmw_interface:
        #     print('App: using IWR6843AoP; starting sensor')
        #     self.mmw_worker.start_mmw()
        #     print('App: done!')
        # else:
        #     print('App: not using IWR6843AoP')

        self.show()

    def init_statistics(self, pos, label):
        vl = init_view(label)
        statistics_ui = {'pid': QLabel(),
                         'ver': QLabel(),
                         'dlen': QLabel(),
                         'numTLVs': QLabel(),
                         'numObj': QLabel(),
                         'pf': QLabel()}
        [v.setText(k) for k, v in statistics_ui.items()]
        [vl.addWidget(v) for v in statistics_ui.values()]
        scene = QGraphicsScene(self)
        spc_gv = QGraphicsView()
        spc_gv.setScene(scene)
        self.figure_gl.addLayout(vl, *pos)

        return vl

    def init_spec_view(self, pos, label):
        display = QGraphicsPixmapItem()
        vl = init_view(label)

        spc_gv = QGraphicsView()
        vl.addWidget(spc_gv)

        self.figure_gl.addLayout(vl, *pos)
        scene = QGraphicsScene(self)
        spc_gv.setScene(scene)
        scene.addItem(display)
        return display

    def init_pts_view(self, pos, label, x_lim, y_lim):
        vl= init_view(label)

        pts_plt = pg.PlotWidget()
        vl.addWidget(pts_plt)

        self.figure_gl.addLayout(vl, *pos)
        pts_plt.setXRange(*x_lim)
        pts_plt.setYRange(*y_lim)
        scatter = pg.ScatterPlotItem(pen=None, symbol='o')
        pts_plt.addItem(scatter)
        return scatter

    def init_curve_view(self, pos, label, x_lim, y_lim):
        vl = init_view(label)

        curve_plt = pg.PlotWidget()
        vl.addWidget(curve_plt)
        self.figure_gl.addLayout(vl, *pos)

        curve_plt.setXRange(*x_lim)
        curve_plt.setYRange(*y_lim)
        curve = curve_plt.plot([], [], pen=pg.mkPen(color=(0, 0, 255)))
        return curve

    def radar_process_mmw_data(self, data_dict):
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
        self.ra_view.setData(range_bin_space, ra)

        # save the data is record is enabled
        # mmw buffer: {'timestamps': [], 'ra_profile': [], 'rd_heatmap': [], 'detected_points': []}
