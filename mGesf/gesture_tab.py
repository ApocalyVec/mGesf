import os
import pickle
import time
from datetime import datetime
import numpy as np

from PyQt5 import QtWidgets, QtGui
from PyQt5 import QtCore

from PyQt5.QtGui import QPixmap
from PyQt5.QtWidgets import QGraphicsPixmapItem, QWidget, QLabel, QGraphicsScene, QGraphicsView, QTabWidget
import pyqtgraph as pg

from utils.img_utils import array_to_colormap_qim

import mGesf.MMW_worker as MMW_worker
from mGesf.drawer import *
import mGesf.config as config
from mGesf.gesture_tabs import DesktopFinger_tab, IndexPen_tab, ThuMouth_tab

class Gesture_tab(QWidget):
    """
    """

    def __init__(self, *args, **kwargs):
        super().__init__()

        self.doppler_display = QGraphicsPixmapItem()

        # #################### create layout #################################

        self.will_recording_radar = False
        self.will_recording_leap = False
        self.will_recording_UWB = False

        self.is_recording_radar = False
        self.is_recording_leap = False
        self.is_recording_UWB = False

        # -------------------- First class --------------------
        # main page
        self.main_page = QtWidgets.QVBoxLayout(self)
        self.setLayout(self.main_page)

        # -------------------- Second class --------------------
        #   1. runtime feature block
        #   2. ITD （index pen, thumouse, desktop at finger) block

        self.runtime_block = init_container(parent=self.main_page, vertical=False)
        # ***** information block *****
        self.ITD_block = init_container(parent=self.main_page, vertical=False)

        # -------------------- third class --------------------
        #   1. Runtime feature block
        #       1-1. radar runtime block
        #       1-2. leap runtime block
        #       1-3. UWB runtime block

        self.radar_runtime_block = init_container(parent=self.runtime_block, vertical=True)
        self.leap_runtime_block = init_container(parent=self.runtime_block, vertical=True)
        self.uwb_runtime_block = init_container(parent=self.runtime_block, vertical=True)

        # -------------------- fourth class -------------------
        #       1-1. radar runtime block
        #           1-1-1. radar runtime graph
        #           1-1-2. radar checkbox

        self.radar_runtime_view = self.init_spec_view(parent=self.radar_runtime_block, label="Radar",
                                                      graph=self.doppler_display)
        self.radar_record_checkbox = setup_check_box(parent=self.radar_runtime_block, function=self.radar_clickBox)

        # -------------------- fourth class -------------------
        #       1-1. leap runtime block
        #           1-1-1. leap runtime graph
        #           1-1-2. leap checkbox

        self.leap_runtime_view = self.init_spec_view(parent=self.radar_runtime_block, label="Leap",
                                                     graph=None)
        self.leap_record_checkbox = setup_check_box(parent=self.radar_runtime_block, function=self.radar_clickBox)

        # -------------------- fourth class -------------------
        #       1-1. UWB runtime block
        #           1-1-1. UWB runtime graph
        #           1-1-2. UWB checkbox
        self.UWB_runtime_view = self.init_spec_view(parent=self.uwb_runtime_block, label="UWB",
                                                    graph=None)
        self.UWB_record_checkbox = setup_check_box(parent=self.uwb_runtime_block, function=self.UWB_clickBox)

        # -------------------- third class --------------------
        #   1. ITD block
        #       1-1. radar runtime block
        #       1-2. leap runtime block
        #       1-3. UWB runtime block

        # Initialize tab screen
        self.tabs = QTabWidget()
        self.tab1 = IndexPen_tab.IndexPen_tab()
        self.tab2 = ThuMouth_tab.ThuMouth_tab()
        self.tab3 = DesktopFinger_tab.DesktopFinger_tab()

        self.tabs.addTab(self.tab1, config.gesture_label1)
        self.tabs.addTab(self.tab2, config.gesture_label2)
        self.tabs.addTab(self.tab3, config.gesture_label3)

        # Add tabs to widget
        self.ITD_block.addWidget(self.tabs)
        self.setLayout(self.ITD_block)

        self.show()

    def init_spec_view(self, parent, label, graph=None):

        if label:
            ql = QLabel()
            ql.setAlignment(QtCore.Qt.AlignTop)
            ql.setAlignment(QtCore.Qt.AlignCenter)
            ql.setText(label)
            parent.addWidget(ql)

        spc_gv = QGraphicsView()
        parent.addWidget(spc_gv)

        scene = QGraphicsScene(self)
        spc_gv.setScene(scene)
        spc_gv.setAlignment(QtCore.Qt.AlignCenter)
        if graph:
            scene.addItem(graph)
        # spc_gv.setFixedSize(config.WINDOW_WIDTH/4, config.WINDOW_HEIGHT/4)
        return scene

    def radar_clickBox(self, state):

        if state == QtCore.Qt.Checked:
            self.will_recording_radar = True
            # self.message.setText(config.radar_box_checked)
        else:
            self.will_recording_radar = False
            # self.message.setText(config.radar_box_unchecked)

    def leap_clickBox(self, state):

        if state == QtCore.Qt.Checked:
            self.will_recording_leap = True
            # self.message.setText(config.leap_box_checked)
        else:
            self.will_recording_leap = False
            # self.message.setText(config.leap_box_unchecked)

    def UWB_clickBox(self, state):

        if state == QtCore.Qt.Checked:
            self.will_recording_UWB = True
            # self.message.setText(config.UWB_box_checked)
        else:
            self.will_recording_UWB = False
            # self.message.setText(config.UWB_box_unchecked)
