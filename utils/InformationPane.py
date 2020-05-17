import config
from utils.GUI_main_window import init_container
from PyQt5 import QtWidgets, QtCore, QtGui
from PyQt5.QtWidgets import QLabel, QCheckBox, QFrame, QVBoxLayout, QFormLayout, QGroupBox, QWidget
from PyQt5.QtCore import Qt, QSize
from datetime import datetime


class InformationPane():
    def __init__(self, parent: QtWidgets.QScrollArea, max_msg_num=1000, label='Console output'):
        infor_pane_size = (int(config.WINDOW_WIDTH / 3), config.WINDOW_HEIGHT)
        information_container = init_container(parent=parent,
                                               label_position="center",
                                               style="background-color: " + config.container_color + ";",
                                               size=infor_pane_size)
        # information_block = init_container(parent=information_container,
        #                                    style="background-color: " + config.subcontainer_color + ";")

        self.formLayout = QFormLayout()
        groupBox = QGroupBox(label)

        groupBox.setLayout(self.formLayout)
        self.timestamp_list = []
        self.comboList = []

        scroll = QtWidgets.QScrollArea()
        scroll.setWidgetResizable(True)
        scroll.setWidget(groupBox)  # set groupBox to be in the scroll
        scroll.setFocusPolicy(QtCore.Qt.NoFocus)
        scroll.ensureWidgetVisible(groupBox)
        scroll.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOn)

        information_container.addWidget(scroll)

        self.max_msg_num = max_msg_num  # TODO remove older messages

    def push(self, msg):
        now = datetime.now()  # current date and time
        row = (QLabel(now.strftime("%m/%d/%Y, %H:%M:%S")), QLabel(msg))
        self.timestamp_list.append(row[0])
        self.comboList.append(row)
        self.formLayout.addRow(row[0], row[1])
