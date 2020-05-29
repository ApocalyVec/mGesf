from PyQt5 import QtGui, QtCore
from PyQt5.QtWidgets import QLabel, QHBoxLayout, QSlider, QMessageBox, QWidget, QVBoxLayout, QScrollArea, QGroupBox, \
    QFormLayout, QGridLayout
from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import QLabel, QHBoxLayout, QSlider

from PyQt5.QtGui import QIcon, QPixmap
import os

from utils.path_utils import get_parent


class Target(QWidget):
    """
    Target circles in ThuMouse recording
    """

    def __init__(self, number=None, label_position='lefttop'):
        super().__init__()
        # Horizontal -> icon | label
        self.layout = QHBoxLayout(self)
        self.setFixedSize(150, 150)

        self.image = QLabel()
        self.image.setAlignment(QtCore.Qt.AlignCenter)
        self.on = False

        # icons
        on_path = os.getcwd() + '/mGesf/resource/figures/circle_on.png'
        off_path = os.getcwd() + '/mGesf/resource/figures/circle_off.png'

        self.on_icon = QPixmap(on_path).scaled(80, 80, Qt.KeepAspectRatio)
        self.off_icon = QPixmap(off_path).scaled(80, 80, Qt.KeepAspectRatio)

        if self.on:
            self.image.setPixmap(self.on_icon)
        else:
            self.image.setPixmap(self.off_icon)

        # resize the target
        self.resize(self.image.width(), self.image.height())

        if number:
            # add the label
            self.label = QLabel()
            self.label.setText(str(number))

            if label_position == "righttop":
                self.layout.addWidget(self.image)
                self.layout.addWidget(self.label)
                self.label.setAlignment(QtCore.Qt.AlignRight)
                self.label.setAlignment(QtCore.Qt.AlignTop)

            else:
                self.layout.addWidget(self.label)
                self.layout.addWidget(self.image)
                self.label.setAlignment(QtCore.Qt.AlignLeft)
                self.label.setAlignment(QtCore.Qt.AlignTop)


    def turn_off(self):
        self.image.setPixmap(self.off_icon)

    def turn_on(self):
        self.image.setPixmap(self.on_icon)
