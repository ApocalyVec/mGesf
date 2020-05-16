from PyQt5.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QSlider, QLabel

from mGesf.main_page_tabs.gesture_tab import help_btn_action
from utils.GUI_main_window import *
from utils.GUI_operation_tab import *
import config


class Detection(QWidget):
    def __init__(self):
        super().__init__()

        # -------------------- First class --------------------
        self.main_page = QHBoxLayout(self)
        self.setLayout(self.main_page)

        self.show()