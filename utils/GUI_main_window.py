from PyQt5 import QtWidgets, QtCore, QtGui
from PyQt5.QtWidgets import QLabel, QCheckBox, QFrame, QVBoxLayout

import config as config


def init_view(label, container, label_bold=True, position="center", vertical=True):
    if vertical:
        vl = QtGui.QVBoxLayout(container)
    else:
        vl = QtGui.QHBoxLayout(container)
    if label:
        ql = QLabel()
        if label_bold:
            ql.setStyleSheet("font: bold 14px;")
        if position == "center":
            ql.setAlignment(QtCore.Qt.AlignTop)
            ql.setAlignment(QtCore.Qt.AlignCenter)
        elif position == "rightbottom":
            ql.setAlignment(QtCore.Qt.AlignRight)
            ql.setAlignment(QtCore.Qt.AlignBottom)

        ql.setText(label)
        vl.addWidget(ql)

    return vl


def init_container(parent, label=None, label_position=None, label_bold=True, vertical=True, style=None):
    container = QtGui.QWidget()

    if style:  # set the style of the container, which takes over the invisible layout
        container.setStyleSheet(style)

    parent.addWidget(container)

    vl = init_view(label, container, label_bold, label_position, vertical)

    return vl


def init_button(parent, label=None, function=None, style=config.button_style_classic):
    btn = QtWidgets.QPushButton(text=label)
    btn.clicked.connect(function)
    parent.addWidget(btn)
    btn.setStyleSheet(config.button_style_classic)

    return btn


def init_inputBox(parent, label=None, label_bold=False, default_input=None):
    block = init_container(parent=parent,
                           label=label,
                           label_bold=label_bold,
                           vertical=False)
    textbox = QtWidgets.QLineEdit()
    textbox.setContentsMargins(5, 0, 0, 0)
    textbox.setText(default_input)
    block.addWidget(textbox)
    textbox.setStyleSheet("background-color:white;")

    return block, textbox


def setup_configPath_block(parent):
    is_valid_config_path = False
    config_textbox = init_inputBox(parent=parent,
                                   label=config.control_tab_config_path_label,
                                   label_bold=True,
                                   default_input=config.control_tab_config_file_path_default)
    return is_valid_config_path, config_textbox


def init_checkBox(parent, label=None, function=None):
    box = QCheckBox(label)
    parent.addWidget(box)
    box.stateChanged.connect(function)

    return box


def draw_boarder(parent, width, height):
    frame = QFrame()
    frame.setFixedSize(int(width), int(height))
    frame.setFrameShape(QFrame.StyledPanel)
    frame.setLineWidth(2)
    frame.setContentsMargins(5, 5, 5, 5)
    parent.addWidget(frame)

    return frame
