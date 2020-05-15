import os
from PyQt5.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QSlider, QLabel, QMessageBox, QGraphicsScene, \
    QGraphicsView
from PyQt5 import QtCore
from PyQt5 import QtGui
from PyQt5.QtGui import QPainter, QBrush, QPen, QTransform
from PyQt5.QtWidgets import QApplication, QMainWindow
import sys
from mGesf.main_page_tabs.gesture_tab import help_btn_action, calc_set
from mGesf.main_page_tabs import ReturnKeyDetectionWidget

from utils.GUI_main_window import *
from utils.GUI_operation_tab import *
from utils.sound import *
import config
import pyqtgraph as pg

class Recording(QWidget):
    def __init__(self):
        super().__init__()
        # -------------------- First class --------------------
        self.main_page = QHBoxLayout(self)
        self.setLayout(self.main_page)

        self.show()
