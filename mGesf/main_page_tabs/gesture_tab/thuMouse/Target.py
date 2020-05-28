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
        self.image = QLabel()
        self.image.setAlignment(QtCore.Qt.AlignCenter)
        self.on = False

        # icons
        on_path = os.getcwd() + '/mGesf/resource/figures/circle_on.png'
        off_path = os.getcwd() + '/mGesf/resource/figures/circle_off.png'

        self.on_icon = QPixmap(on_path).scaled(40, 40, Qt.KeepAspectRatio)
        self.off_icon = QPixmap(off_path).scaled(400, 400, Qt.KeepAspectRatio)

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
                self.label.setAlignment(QtCore.Qt.AlignRight)
                self.label.setAlignment(QtCore.Qt.AlignTop)

            elif label_position == "lefttop":
                self.label.setAlignment(QtCore.Qt.AlignLeft)
                self.label.setAlignment(QtCore.Qt.AlignTop)

            self.layout.addWidget(self.label)

    def turn_off(self):
        self.image.setPixmap(self.off_icon)

    def turn_on(self):
        self.image.setPixmap(self.on_icon)
