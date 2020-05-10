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


def setup_radar_connection_button(parent, function):
    btn = QtWidgets.QPushButton(text='Connect')
    btn.clicked.connect(function)
    parent.addWidget(btn)
    btn.setStyleSheet(config.button_style)
    return btn


def setup_user_port(parent):
    user_port_block = init_container(parent=parent, label='User Port (Enhanced): ', label_bold=False,
                                     vertical=False)
    uport_textbox = QtWidgets.QLineEdit()
    uport_textbox.setContentsMargins(5, 0, 0, 0)
    uport_textbox.setText(config.u_port_default)
    user_port_block.addWidget(uport_textbox)
    uport_textbox.setStyleSheet("background-color:white;")

    return user_port_block, uport_textbox


def setup_data_port(parent):
    data_port_block = init_container(parent=parent, label='Data Port (Standard): ', label_bold=False,
                                     vertical=False)
    dport_textbox = QtWidgets.QLineEdit()
    dport_textbox.setContentsMargins(10, 0, 0, 0)
    dport_textbox.setText(config.d_port_default)
    data_port_block.addWidget(dport_textbox)
    dport_textbox.setStyleSheet("background-color:white;")

    return data_port_block, dport_textbox


def setup_datapath_block(parent):
    data_path_block = init_container(parent=parent, label='Output path: ', vertical=False)
    data_path_textbox = QtWidgets.QLineEdit()
    data_path_textbox.setContentsMargins(10, 0, 0, 0)
    data_path_textbox.setText(config.data_path)
    data_path_block.addWidget(data_path_textbox)
    data_path_textbox.setStyleSheet("background-color:white;")

    return data_path_block, data_path_textbox


def setup_information_block(parent):
    information_block1 = init_container(parent=parent, label="Information", label_position="center",
                                        style="background-color: " + config.container_color + ";")
    information_block = init_container(parent=information_block1,
                                       style="background-color: " + config.subcontainer_color + ";")
    message = QLabel()
    message.setText("Hi there")
    information_block.addWidget(message)

    # widget for the scroll area
    info_widget = QtGui.QWidget()
    information_block1.addWidget(info_widget)
    info_layout = QVBoxLayout()
    info_widget.setLayout(info_layout)

    # Scroll area
    scroll_area = QtWidgets.QScrollArea()
    scroll_area.setWidgetResizable(True)
    scroll_area.setWidget(info_widget)
    scroll_area.setFocusPolicy(QtCore.Qt.NoFocus)

    # Scroll Area Content widget
    content = QtWidgets.QWidget(scroll_area)
    scroll_area.ensureWidgetVisible(info_widget)
    content.setFocusPolicy(QtCore.Qt.NoFocus)

    # add a layout to put in message
    scroll_layout = QVBoxLayout(content)
    scroll_layout.setSpacing(20.0)

    message = QLabel()
    text = "# Scroll Area Content\nscrollArea_content = QtWidgets.QWidget(scrollArea)" \
           "\nscrollArea.ensureWidgetVisible(scrollArea_content)\n" \
           " scrollArea_content.setFocusPolicy(QtCore.Qt.NoFocus)"

    message.setText(text)
    message.setLayout(scroll_layout)
    return scroll_area, message
  

def setup_record_button(parent, function):
    record_btn = QtWidgets.QPushButton(text='Start Recording')
    record_btn.clicked.connect(function)
    parent.addWidget(record_btn)
    record_btn.setStyleSheet(config.button_style)
    return record_btn


def setup_config_path_block(parent):
    is_valid_config_path = False
    config_textbox = QtWidgets.QLineEdit()
    config_textbox.setText(config.config_file_path_default)
    parent.addWidget(config_textbox)
    config_textbox.setStyleSheet("background-color:white;")
    return is_valid_config_path, config_textbox


def setup_config_btn(parent, function):
    config_connection_btn = QtWidgets.QPushButton(text='Send Config')
    config_connection_btn.clicked.connect(function)
    parent.addWidget(config_connection_btn)
    config_connection_btn.setStyleSheet(config.button_style)

    return config_connection_btn


def setup_sensor_btn(parent, function):
    sensor_start_stop_btn = QtWidgets.QPushButton(text='Start Sensor')
    sensor_start_stop_btn.clicked.connect(function)
    parent.addWidget(sensor_start_stop_btn)
    sensor_start_stop_btn.setStyleSheet(config.button_style)
    return sensor_start_stop_btn


def setup_check_box(parent, function):
    box = QCheckBox("Record")
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
