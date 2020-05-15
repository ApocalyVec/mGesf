from PyQt5.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QSlider, QLabel
from PyQt5.QtCore import Qt

from mGesf.main_page_tabs.gesture_tab import help_btn_action
from utils.GUI_main_window import *
from utils.GUI_operation_tab import *
import config


class Train(QWidget):
    def __init__(self):
        super().__init__()
        # -------------------- First class --------------------
        self.main_page = QHBoxLayout(self)
        self.setLayout(self.main_page)

        self.show()