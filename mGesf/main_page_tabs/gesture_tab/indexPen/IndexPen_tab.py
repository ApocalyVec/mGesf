from PyQt5.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout

from utils.GUI_gesture_tab import setup_operation_tabs
from utils.GUI_main_window import init_container


class IndexPen_tab(QWidget):
    def __init__(self, *args, **kwargs):
        super().__init__()
        self.main_page = QHBoxLayout(self)

        self.operation_block = init_container(parent=self.main_page, vertical=False)
        setup_operation_tabs(self.operation_block)

        self.setLayout(self.main_page)
        self.show()


