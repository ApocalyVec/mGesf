from PyQt5 import QtWidgets, QtCore
from PyQt5.QtWidgets import QWidget, QVBoxLayout, QLabel
import pyqtgraph as pg

import mGesf.workers as Leap_worker

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

        main_page = QtWidgets.QVBoxLayout(self)
        self.setLayout(main_page)
        self.figure_gl = QtWidgets.QGridLayout()  # create grid layout for the figures
        self.info_vl = QtWidgets.QVBoxLayout()  # create vbox for controls
        main_page.addLayout(self.figure_gl)
        main_page.addLayout(self.info_vl)

        self.layout = QVBoxLayout(self)

        self.H = self.init_curve_view(pos=(0, 0),
                                      label='Horizontal Direction',
                                      x_lim=(-0.5, 0.5),
                                      y_lim=(0, 1.))

        self.V = self.init_curve_view(pos=(1, 0),
                                      label='Vertical Direction',
                                      x_lim=(-0.5, 0.5),
                                      y_lim=(0, 1.))

        self.Z = self.init_curve_view(pos=(2, 0),
                                      label='Z Direction',
                                      x_lim=(-0.5, 0.5),
                                      y_lim=(-1., 1.))

        self.leap_worker.signal_leap.connect(self.leap_process_data)

        self.show()

    def init_curve_view(self, pos, label, x_lim, y_lim):
        vl = init_view(label)

        curve_plt = pg.PlotWidget()
        vl.addWidget(curve_plt)
        self.figure_gl.addLayout(vl, *pos)

        curve_plt.setXRange(*x_lim)
        curve_plt.setYRange(*y_lim)
        curve = curve_plt.plot([], [], pen=pg.mkPen(color=(0, 0, 255)))
        return curve

    def leap_process_data(self, data_dict):
        a = 1

    def set_fire_tab_signal(self, is_fire_signal):
        pass
