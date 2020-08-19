from PyQt5 import QtWidgets
from PyQt5.QtWidgets import QWidget
from utils.GUI_main_window import init_container


class XeThruX4Tab(QWidget):
    def __init__(self, *args, **kwargs):
        super().__init__()

        self.main_page = QtWidgets.QVBoxLayout(self)
        self.setLayout(self.main_page)
        self.graph_container1 = init_container(parent=self.main_page,
                                               vertical=True,
                                               label="BASEBAND AMPLITUDE",
                                               label_bold=True,
                                               label_position="lefttop")

        self.graph_container2 = init_container(parent=self.main_page,
                                               vertical=True,
                                               label="BASEBAND PHASE",
                                               label_bold=True,
                                               label_position="lefttop")

        self.graph_container3 = init_container(parent=self.main_page,
                                               vertical=True,
                                               label="BASEBAND HISTORY",
                                               label_bold=True,
                                               label_position="lefttop")

        self.show()
