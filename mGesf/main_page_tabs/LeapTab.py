from PyQt5 import QtWidgets, QtCore
from PyQt5.QtWidgets import QWidget, QVBoxLayout, QLabel
import pyqtgraph as pg

import mGesf.workers as Leap_worker

from collections import deque

from utils.GUI_main_window import *


def init_view(label):
    vl = QtWidgets.QVBoxLayout()
    ql = QLabel()
    ql.setAlignment(QtCore.Qt.AlignCenter)
    ql.setText(label)
    vl.addWidget(ql)
    return vl

class LeapTab(QWidget):
    def __init__(self, leap_worker: Leap_worker, refresh_interval, *args, **kwargs):
        super().__init__()

        self.leap_worker = leap_worker

        self.main_page = QtWidgets.QHBoxLayout(self)
        self.setLayout(self.main_page)
        # self.figure_gl = QtWidgets.QGridLayout()  # create grid layout for the figures
        # self.info_vl = QtWidgets.QVBoxLayout()  # create vbox for controls
        # main_page.addLayout(self.figure_gl)
        # main_page.addLayout(self.info_vl)

        self.layout = QVBoxLayout(self)


        self.v_data = deque(maxlen=100)
        self.h_data = deque(maxlen=100)
        self.z_data = deque(maxlen=100)


        self.graph_container1 = init_container(parent=self.main_page,
                                               vertical=True,
                                               label="RF Frame and Baseband Frame",
                                               label_bold=True,
                                               label_position="lefttop")

        self.graph_container2 = init_container(parent=self.main_page,
                                               vertical=True,
                                               label="Clutter Removed frame",
                                               label_bold=True,
                                               label_position="lefttop")
        self.a_curve = self.init_leap_runtime_view(parent=self.graph_container1, label="")


        self.scatter_container = init_container(parent=self.graph_container2,
                                          vertical=True,
                                          label="",
                                          label_bold=True,
                                          label_position="lefttop")

        self.leap_scatter = self.init_leap_scatter(parent=self.scatter_container, label="LeapMouse")

        self.graph_container3 = init_container(parent=self.graph_container2,
                                               vertical=True,
                                               label="Clutter Removed frame",
                                               label_bold=True,
                                               label_position="lefttop")


        # self.V_container = init_container(parent=self.graph_container3,
        #                                        vertical=True,
        #                                        label="v",
        #                                        label_bold=True,
        #                                        label_position="lefttop")
        #
        # self.H_container = init_container(parent=self.graph_container3,
        #                                        vertical=True,
        #                                        label="h",
        #                                        label_bold=True,
        #                                        label_position="lefttop")
        #
        # self.Z_container = init_container(parent=self.graph_container3,
        #                                        vertical=True,
        #                                        label="z",
        #                                        label_bold=True,
        #                                        label_position="lefttop")


        self.V_curve = self.init_leap_runtime_view(parent=self.graph_container3, label="V")
        self.H_curve = self.init_leap_runtime_view(parent=self.graph_container3, label="H")
        self.Z_curve = self.init_leap_runtime_view(parent=self.graph_container3, label="Z")


        # self.V_curve = self.init_curve_view(pos=(0, 0),
        #                               label='Vertical Direction',
        #                               x_lim=(0, 100),
        #                               y_lim=(-0.03, 0.03))
        #
        # self.H_curve = self.init_curve_view(pos=(1, 0),
        #                               label='Horizontal Direction',
        #                               x_lim=(0, 100),
        #                               y_lim=(-0.03, 0.03))
        #
        # self.Z_curve = self.init_curve_view(pos=(2, 0),
        #                               label='Z Direction',
        #                               x_lim=(0, 100),
        #                               y_lim=(-0.03, 0.03))
        #
        self.leap_worker.signal_leap.connect(self.leap_process_data)

        self.show()

    # def init_curve_view(self, pos, label, x_lim, y_lim):
    #     vl = init_view(label)
    #
    #     curve_plt = pg.PlotWidget()
    #     vl.addWidget(curve_plt)
    #     self.figure_gl.addLayout(vl, *pos)
    #
    #     curve_plt.setXRange(*x_lim)
    #     curve_plt.setYRange(*y_lim)
    #     curve = curve_plt.plot([], [], pen=pg.mkPen(color=(0, 0, 255)))
    #     return curve


    def init_leap_runtime_view(self, parent, label):
        if label:
            ql = QLabel()
            ql.setAlignment(QtCore.Qt.AlignTop)
            ql.setAlignment(QtCore.Qt.AlignCenter)
            ql.setText(label)
            parent.addWidget(ql)
        position = pg.PlotWidget()
        parent.addWidget(position)

        # rf_frame.setXRange(*x_lim)
        # rf_frame.setYRange(*y_lim)

        pen = pg.mkPen(color=(0, 0, 255), width=1)
        p = position.plot([], [], pen=pen, name="position")

        return p

    def leap_process_data(self, data_dict):
        if data_dict is not None:
            data_dict['leapmouse'] = data_dict['leapmouse'].split(' ')
            data_dict['leapmouse'] = [float(value) for value in data_dict['leapmouse']]
            self.v_data.append(data_dict['leapmouse'][0])
            self.h_data.append(data_dict['leapmouse'][1])
            self.z_data.append(data_dict['leapmouse'][2])
            xsamples = list(range(len(self.v_data)))
            self.V_curve.setData(xsamples, list(self.v_data))
            self.H_curve.setData(xsamples, self.h_data)
            self.Z_curve.setData(xsamples, self.z_data)

            new_x_pos, new_y_pos = data_dict['leapmouse'][3], data_dict['leapmouse'][4]
            self.leap_scatter.setData([new_x_pos], [new_y_pos])

    def init_leap_scatter(self, parent, label):
        if label:
            ql = QLabel()
            ql.setAlignment(QtCore.Qt.AlignTop)
            ql.setAlignment(QtCore.Qt.AlignCenter)
            ql.setText(label)
            parent.addWidget(ql)

        pts_plt = pg.PlotWidget()
        parent.addWidget(pts_plt)
        pts_plt.setXRange(-0.1, 0.1)
        pts_plt.setYRange(1.5, 1.7)
        scatter = pg.ScatterPlotItem(pen=None, symbol='o')
        pts_plt.addItem(scatter)
        return scatter


