from PyQt5.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout

from utils.gesture_tabs_GUI_utils import setup_operation_tabs
from utils.main_window_GUI_util import init_container


class ThuMouth_tab(QWidget):
    def __init__(self):
        super().__init__()
        self.main_page = QHBoxLayout(self)

        self.operation_block = init_container(parent=self.main_page, vertical=False)
        setup_operation_tabs(self.operation_block)

        self.setLayout(self.main_page)
        self.show()
