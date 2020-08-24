from PyQt5 import QtWidgets, QtCore
from PyQt5.QtGui import QPixmap
from PyQt5.QtWidgets import QWidget, QLabel, QGraphicsPixmapItem, QGraphicsView, QGraphicsScene
import pyqtgraph as pg

from mGesf import workers
from utils.GUI_main_window import init_container
from utils.img_utils import array_to_colormap_qim


class XeThruX4Tab(QWidget):
    def __init__(self, Xe4Thru_worker: workers.Xe4ThruWorker, *args, **kwargs):
        super().__init__()

        self.Xe4Thru_worker = Xe4Thru_worker
        self.Xe4Thru_worker.signal_data.connect(self.control_process_xethru_data)

        self.main_page = QtWidgets.QVBoxLayout(self)
        self.setLayout(self.main_page)
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

        self.graph_container3 = init_container(parent=self.main_page,
                                               vertical=True,
                                               label="BASEBAND HISTORY",
                                               label_bold=True,
                                               label_position="lefttop")

        # plots
        self.rf_curve, self.baseband_curve = self.init_xethrux4_runtime_view(parent=self.graph_container1,
                                                                             label="Clutter frame")
        self.clutter_free_rf_curve, self.clutter_free_baseband_curve = self.init_xethrux4_runtime_view(
            parent=self.graph_container2, label="Clutter free frame")

        self.xethrux4_ir_spectrogram_display = QGraphicsPixmapItem()
        self.xethrux4_runtime_view = self.init_spec_view(parent=self.graph_container3, label="XeThruX4",
                                                         graph=self.xethrux4_ir_spectrogram_display)
        self.show()

    def init_xethrux4_runtime_view(self, parent, label):
        if label:
            ql = QLabel()
            ql.setAlignment(QtCore.Qt.AlignTop)
            ql.setAlignment(QtCore.Qt.AlignCenter)
            ql.setText(label)
            parent.addWidget(ql)
        rf_frame = pg.PlotWidget()
        parent.addWidget(rf_frame)

        # rf_frame.setXRange(*x_lim)
        # rf_frame.setYRange(*y_lim)

        pen = pg.mkPen(color=(0, 0, 255), width=1)
        rf_curve = rf_frame.plot([], [], pen=pen, name="rf_curve")

        pen = pg.mkPen(color=(255, 0, 0), width=2)
        baseband = rf_frame.plot([], [], pen=pen, name="baseband_curve")

        return rf_curve, baseband

    @QtCore.pyqtSlot(dict)
    def control_process_xethru_data(self, data_dict):
        if data_dict['frame'] is not None:
            xsamples = list(range(data_dict['frame'].shape[0]))
            rf_frame = data_dict['frame']
            baseband_frame = data_dict['baseband_frame']
            clutter_removal_rf_frame = data_dict['clutter_removal_frame']
            clutter_removal_baseband_frame = data_dict['clutter_removal_baseband_frame']

            self.rf_curve.setData(xsamples, rf_frame)
            self.baseband_curve.setData(xsamples, baseband_frame)
            self.clutter_free_rf_curve.setData(xsamples, clutter_removal_rf_frame)
            self.clutter_free_baseband_curve.setData(xsamples, clutter_removal_baseband_frame)

            ir_heatmap_qim = array_to_colormap_qim(data_dict['ir_spectrogram'])
            ir_qpixmap = QPixmap(ir_heatmap_qim)
            ir_qpixmap = ir_qpixmap.scaled(500, 8000, pg.QtCore.Qt.KeepAspectRatio)  # resize spectrogram
            self.xethrux4_ir_spectrogram_display.setPixmap(ir_qpixmap)

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
