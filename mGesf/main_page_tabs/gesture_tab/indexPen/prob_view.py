import PyQt5
from PyQt5.QtCore import pyqtSignal
from PyQt5.QtWidgets import QMainWindow, QVBoxLayout, QLabel, QHBoxLayout, QFormLayout
import pyqtgraph as pg
from sklearn.preprocessing import OneHotEncoder

import config
from utils.GUI_main_window import init_view
from utils.GUI_operation_tab import init_scroll_area, clear_layout, create_plot_widget
import numpy as np

class ProbViewWindow(QMainWindow):
    signal_dtc_output = pyqtSignal(np.ndarray)

    def __init__(self, parent=None, max_timesteps=1000):  # TODO add this maxt timesteps to the GUI
        super(ProbViewWindow, self).__init__(parent)
        self.title = 'Probability View'
        self.resize(config.prob_view_window_size[0], config.prob_view_window_size[1])
        self.prob_form, scroll = init_scroll_area(parent=self.layout(), label='Temporal Probability Evolution',
                                                  size=config.prob_view_window_size)
        # align=PyQt5.QtCore.Qt.AlignLeft)
        self.setCentralWidget(scroll)
        self.prob_plots = []
        self.signal_dtc_output.connect(self.update_probs)

        # prob matrix; timesteps * classes
        self.max_timesteps = max_timesteps
        self.prob_mat = None

    def setup(self, encoder: OneHotEncoder):
        self.reset()
        self.prob_mat = np.zeros(shape=(0, len(encoder.categories_[0])))
        # self.prob_mat = np.zeros(shape=(0, 121, 1))

        for c in encoder.categories_[0]:
            prob_widget, prob_plt = create_plot_widget(y_lim=(0., 1.))
            self.prob_plots.append(prob_plt)
            self.prob_form.addRow(QLabel(c), prob_widget)
            # self.prob_form.addRow(QLabel(c), QLabel('THIS'))

    def reset(self):
        clear_layout(self.layout())
        self.prob_plots = []

    def update_probs(self, dtc_output):
        self.prob_mat: np.ndarray = np.concatenate([self.prob_mat, dtc_output], axis=0)[-self.max_timesteps:]
        time_step_bins = np.asarray(range(len(self.prob_mat)))
        for probs, plot in zip(self.prob_mat.transpose(), self.prob_plots):
            plot.setData(time_step_bins, probs)