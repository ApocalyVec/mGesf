import os
import pickle
import time
from datetime import datetime

from PyQt5 import QtWidgets
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
    return vl, ql


class Radar_tab(QWidget):
    def __init__(self):
        super().__init__()

        main_page = QtWidgets.QVBoxLayout(self)
        self.setLayout(main_page)
        self.figure_gl = QtWidgets.QGridLayout()  # create grid layout for the figures
        self.info_vl = QtWidgets.QVBoxLayout()  # create vbox for controls
        main_page.addLayout(self.figure_gl)
        main_page.addLayout(self.info_vl)


        # ====================== Add graphs to grids =======================================

        self.init_spec_view(pos=(0, 0), label='Statistics')
        self.init_spec_view(pos=(0, 1), label='Detected Point XY')
        self.init_spec_view(pos=(0, 2), label='Detected Point Z')
        self.init_spec_view(pos=(1, 0), label='Range Profile')
        self.init_spec_view(pos=(1, 1), label='Rd Heat Map')
        self.init_spec_view(pos=(1, 2), label='RA Heat Map')

        # ====================== Add info to info_vl =======================================
        self.info_label = QLabel()
        self.info_label.setText("Information: ")
        self.info_vl.addWidget(self.info_label)

    def init_spec_view(self, pos, label):
        vl, ql = init_view(label)

        spc_gv = QGraphicsView()
        vl.addWidget(spc_gv)

        self.figure_gl.addLayout(vl, *pos)
        scene = QGraphicsScene(self)
        spc_gv.setScene(scene)
        # scene.addItem(self.doppler_display)
        return scene
