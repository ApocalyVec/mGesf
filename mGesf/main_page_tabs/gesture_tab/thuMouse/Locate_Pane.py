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

        # initialize 4 targets and put them to four corners
        self.target1 = Target(number=1, label_position="righttop")
        self.target2 = Target(number=2, label_position="righttop")
        self.target3 = Target(number=3, label_position="lefttop")
        self.target4 = Target(number=4, label_position="lefttop")

        self.target_w1, self.target_w2, self.target_w3, self.target_w4 = self.set_targets_to_corners()

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
        parent.addWidget(self.target_w1, 0, 0)
        parent.addWidget(self.target_w2, rows - 1, 0)
        parent.addWidget(self.target_w3, 0, columns - 1)
        parent.addWidget(self.target_w4, rows - 1, columns - 1)

    def activate(self):
        # randomly turn on one target when hit
        self.activated_target = randint(1, 4)
        self.targets[self.activated_target].turn_on()

    def random_switch(self):
        # turn off the current one
        self.targets[self.activated_target].turn_off()
        # turn on a new one
        self.activate()

    def set_targets_to_corners(self):
        # TODO THIS IS NOT WORKING PROPERLY
        widget1 = QWidget()
        widget2 = QWidget()
        widget3 = QWidget()
        widget4 = QWidget()

        layout1 = QVBoxLayout(widget1)
        layout2 = QVBoxLayout(widget2)
        layout3 = QVBoxLayout(widget3)
        layout4 = QVBoxLayout(widget4)

        layout1.addWidget(self.target1)
        layout1.setAlignment(QtCore.Qt.AlignLeft)
        layout1.setAlignment(QtCore.Qt.AlignTop)

        layout2.addWidget(self.target2)
        layout2.setAlignment(QtCore.Qt.AlignLeft)
        layout2.setAlignment(QtCore.Qt.AlignBottom)

        layout3.addWidget(self.target3)
        layout3.setAlignment(QtCore.Qt.AlignRight)
        layout3.setAlignment(QtCore.Qt.AlignTop)

        layout4.addWidget(self.target4)
        layout4.setAlignment(QtCore.Qt.AlignRight)
        layout4.setAlignment(QtCore.Qt.AlignBottom)

        return widget1, widget2, widget3, widget4
