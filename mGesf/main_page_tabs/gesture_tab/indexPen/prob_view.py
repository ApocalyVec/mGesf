import PyQt5
from PyQt5.QtWidgets import QMainWindow, QVBoxLayout, QLabel, QHBoxLayout, QFormLayout
import pyqtgraph as pg
import config
from utils.GUI_main_window import init_view
from utils.GUI_operation_tab import init_scroll_area, clear_layout


class ProbViewWindow(QMainWindow):
    def __init__(self, parent=None):
        super(ProbViewWindow, self).__init__(parent)
        self.title = 'Probability View'
        self.resize(config.prob_view_window_size[0], config.prob_view_window_size[1])
        prob_form: QFormLayout = init_scroll_area(parent=self.layout(), label='Temporal Probability Evolution',
                                                  size=config.prob_view_window_size)
                                                  # align=PyQt5.QtCore.Qt.AlignLeft)
        for i in range(10):
            prob_form.addRow(QLabel(str(i)), QLabel('This'))

    def setup(self, encoder):  # TODO implement setup
        clear_layout(self.layout())


    def init_curve_view(self, pos, label, x_lim, y_lim):
        vl = init_view(label)

        curve_plt = pg.PlotWidget()
        vl.addWidget(curve_plt)
        self.figure_gl.addLayout(vl, *pos)

        curve_plt.setXRange(*x_lim)
        curve_plt.setYRange(*y_lim)
        curve = curve_plt.plot([], [], pen=pg.mkPen(color=(0, 0, 255)))
        return curve