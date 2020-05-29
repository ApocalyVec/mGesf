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

    def __init__(self, parent, rows=4, columns=4):
        """
        :param parent: A grid-layout
        :param rows: number of rows in the grid
        :param columns: number of columns in the grid

        # WIDGET1      |      WIDGET3
        #              |
        #              |
        # ---------------------------
        #              |
        #              |
        # WIDGET2      |      WIDGET4

        - interactive window widget
            - grid-layout of size (rows-1 * columns-1)
                - widget1
                    - target1
                - widget2
                     - target2
                - widget3
                     - target3
                - widget4
                     - target4
        """
        super().__init__()

        # initialize 4 targets and put them to four corners
        self.target1 = Target(number=1, label_position="righttop")
        self.target2 = Target(number=2, label_position="righttop")
        self.target3 = Target(number=3, label_position="lefttop")
        self.target4 = Target(number=4, label_position="lefttop")

        # widgets that hold the 4 targets
        # used for setsize()
        self.target_w1, self.target_w2, self.target_w3, self.target_w4 = self.set_targets_to_corners()

        # for quick reference and change target on/off
        self.targets = {1: self.target1,
                        2: self.target2,
                        3: self.target3,
                        4: self.target4}

        # which target is now activated?
        self.activated_target = None

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
        def _set_to_corner(layout, target, p1, p2):
            layout.addWidget(target)
            layout.setAlignment(p1)
            layout.setAlignment(p2)

        # TODO THIS IS NOT WORKING PROPERLY
        widget1 = QWidget()
        widget2 = QWidget()
        widget3 = QWidget()
        widget4 = QWidget()

        layout1 = QVBoxLayout(widget1)
        layout2 = QVBoxLayout(widget2)
        layout3 = QVBoxLayout(widget3)
        layout4 = QVBoxLayout(widget4)

        _set_to_corner(layout1, self.target1, QtCore.Qt.AlignLeft, QtCore.Qt.AlignTop)
        _set_to_corner(layout2, self.target2, QtCore.Qt.AlignLeft, QtCore.Qt.AlignBottom)
        _set_to_corner(layout3, self.target3, QtCore.Qt.AlignRight, QtCore.Qt.AlignTop)
        _set_to_corner(layout4, self.target4, QtCore.Qt.AlignRight, QtCore.Qt.AlignBottom)

        return widget1, widget2, widget3, widget4
