from PyQt5 import QtGui, QtCore
from PyQt5.QtWidgets import QLabel, QHBoxLayout, QSlider, QMessageBox, QWidget, QVBoxLayout, QScrollArea, QGroupBox, \
    QFormLayout, QGridLayout
from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import QLabel, QHBoxLayout, QSlider

from PyQt5.QtGui import QIcon, QPixmap
import os
from mGesf.main_page_tabs.gesture_tab.thuMouse.Target import Target
from random import randint


class Locate_Pane(QWidget):
    """
    The locate canvas
    """

    def __init__(self, parent, rows=4, columns=4):
        super().__init__()
        self.container = QtGui.QWidget()
        parent.addWidget(self.container)
        self.layout = QGridLayout(self.container)
        # initialize 4 targets and put them to four corners
        self.target1 = Target(number=1, label_position="righttop")
        self.target2 = Target(number=2, label_position="righttop")
        self.target3 = Target(number=3, label_position="lefttop")
        self.target4 = Target(number=4, label_position="lefttop")

        self.targets = {1: self.target1,
                        2: self.target2,
                        3: self.target3,
                        4: self.target4}

        # which target is now activated?
        self.activated_target = None

        # 1      |      3
        #        |
        # ---------------
        #        |
        # 2      |      4
        self.layout.addWidget(self.target1, 0, 0)
        self.layout.addWidget(self.target2, rows - 1, 0)
        self.layout.addWidget(self.target3, 0, columns - 1)
        self.layout.addWidget(self.target4, rows - 1, columns - 1)

    def activate(self):
        # randomly turn on one target when hit
        self.activated_target = randint(1, 4)
        self.targets[self.activated_target].turn_on()

    def random_switch(self):
        # turn off the current one
        self.targets[self.activated_target].turn_off()
        # turn on a new one
        self.activate()
