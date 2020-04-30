from PyQt5 import QtWidgets, QtCore, QtGui
from PyQt5.QtWidgets import QLabel, QCheckBox, QFrame

from mGesf import config as config


def init_view(label, container, position="center", vertical=True):
    if vertical:
        vl = QtGui.QVBoxLayout(container)
    else:
        vl = QtGui.QHBoxLayout(container)
    if label:
        ql = QLabel()
        if position == "center":
            ql.setAlignment(QtCore.Qt.AlignTop)
            ql.setAlignment(QtCore.Qt.AlignCenter)
        elif position == "rightbottom":
            ql.setAlignment(QtCore.Qt.AlignRight)
            ql.setAlignment(QtCore.Qt.AlignBottom)

        ql.setText(label)
        vl.addWidget(ql)

    return vl


def init_container(parent, label=None, label_position=None, vertical=True, style=None):
    container = QtGui.QWidget()
    if style:  # set the style of the container, which takes over the invisible layout
        container.setStyleSheet(style)

    parent.addWidget(container)
    vl = init_view(label, container, label_position, vertical)

    # widget_contrainer.addLayout(vl)

    return vl


def setup_radar_connection_button(parent, function):
    btn = QtWidgets.QPushButton(text='Connect')
    btn.clicked.connect(function)
    parent.addWidget(btn)
    return btn


def setup_user_port(parent):
    user_port_block = init_container(parent=parent, label='User Port (Enhanced)',
                                     vertical=False)
    uport_textbox = QtWidgets.QLineEdit()
    uport_textbox.setContentsMargins(5, 0, 0, 0)
    uport_textbox.setPlaceholderText("default: " + config.u_port_default)
    user_port_block.addWidget(uport_textbox)
    return user_port_block, uport_textbox


def setup_data_port(parent):
    data_port_block = init_container(parent=parent, label='Data Port (Standard)',
                                     vertical=False)
    dport_textbox = QtWidgets.QLineEdit()
    dport_textbox.setContentsMargins(10, 0, 0, 0)
    dport_textbox.setPlaceholderText("default: " + config.d_port_default)
    data_port_block.addWidget(dport_textbox)
    return data_port_block, dport_textbox


def setup_datapath_block(parent):
    data_path_block = init_container(parent=parent, label='Output path: ', vertical=False)
    data_path_textbox = QtWidgets.QLineEdit()
    data_path_textbox.setContentsMargins(10, 0, 0, 0)
    data_path_textbox.setPlaceholderText("default: " + config.data_path)
    data_path_block.addWidget(data_path_textbox)

    return data_path_block, data_path_textbox


def setup_information_block(parent):
    information_block = init_container(parent=parent, label="Information")
    message = QLabel()
    message.setText("Hi there")
    information_block.addWidget(message)

    return information_block, message


def setup_record_button(parent, function):
    record_btn = QtWidgets.QPushButton(text='Start Recording')
    record_btn.clicked.connect(function)
    parent.addWidget(record_btn)

    return record_btn


def setup_config_path_block(parent):
    is_valid_config_path = False
    config_textbox = QtWidgets.QLineEdit()
    config_textbox.setPlaceholderText('default ' + config.config_file_path_default)
    parent.addWidget(config_textbox)

    return is_valid_config_path, config_textbox


def setup_config_btn(parent, function):
    config_connection_btn = QtWidgets.QPushButton(text='Send Config')
    config_connection_btn.clicked.connect(function)
    parent.addWidget(config_connection_btn)

    return config_connection_btn


def setup_sensor_btn(parent, function):
    sensor_start_stop_btn = QtWidgets.QPushButton(text='Start Sensor')
    sensor_start_stop_btn.clicked.connect(function)
    parent.addWidget(sensor_start_stop_btn)

    return sensor_start_stop_btn


def setup_check_box(parent, function):
    box = QCheckBox("checkbox")
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
