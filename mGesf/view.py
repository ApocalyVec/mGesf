import pyqtgraph as pg
from PyQt5 import QtWidgets
from PyQt5.QtWidgets import QGraphicsScene, QGraphicsView, QLabel


def init_statistics(figure_gl):
    statistics_vl = QtWidgets.QVBoxLayout()
    statistics_ui = {'pid': QLabel(),
                     'ver': QLabel(),
                     'dlen': QLabel(),
                     'numTLVs': QLabel(),
                     'numObj': QLabel(),
                     'pf': QLabel()}
    [v.setText(k) for k, v in statistics_ui.items()]
    [statistics_vl.addWidget(v) for v in statistics_ui.values()]
    figure_gl.addLayout(statistics_vl, *(0, 0))  # why does not this show up

    return statistics_ui, statistics_vl


def init_view(label):
    vl = QtWidgets.QVBoxLayout()
    ql = QLabel()
    ql.setText(label)
    vl.addWidget(ql)
    return vl, ql


def init_spec(window, pos, label):
    vl, ql = init_view(label)

    spc_gv = QGraphicsView()
    vl.addWidget(spc_gv)

    window.figure_gl.addLayout(vl, *pos)
    scene = QGraphicsScene(window)
    spc_gv.setScene(scene)
    scene.addItem(window.doppler_display)
    return scene


def init_pts_view(window, pos, label, x_lim, y_lim):
    vl, ql = init_view(label)

    pts_plt = pg.PlotWidget()
    vl.addWidget(pts_plt)

    window.figure_gl.addLayout(vl, *pos)
    pts_plt.setXRange(*x_lim)
    pts_plt.setYRange(*y_lim)
    scatter = pg.ScatterPlotItem(pen=None, symbol='o')
    pts_plt.addItem(scatter)
    return scatter


def init_curve(window, pos, label, x_lim, y_lim):
    vl, ql = init_view(label)

    curve_plt = pg.PlotWidget()
    vl.addWidget(curve_plt)
    window.figure_gl.addLayout(vl, *pos)

    curve_plt.setXRange(*x_lim)
    curve_plt.setYRange(*y_lim)
    curve = curve_plt.plot([], [], pen=pg.mkPen(color=(0, 0, 255)))
    return curve
