from PyQt5 import QtGui, QtCore
from PyQt5.QtWidgets import QLabel, QHBoxLayout, QSlider, QMessageBox, QWidget
from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import QLabel, QHBoxLayout, QSlider

from utils.GUI_main_window import init_container, init_view
from utils.labeled_Slider import LabeledSlider
from PyQt5.QtGui import QIcon, QPixmap


def init_slider_bar_box(parent, label=None, interval=5):
    line_container = init_container(parent=parent, vertical=False)

    if label:
        ql = QLabel()
        ql.setStyleSheet("font: 14px;")
        ql.setText(label)
        line_container.addWidget(ql)

    # slider with interval
    slider_view = LabeledSlider(minimum=1, maximum=interval)
    slider_view.setFocusPolicy(Qt.StrongFocus)

    line_container.addWidget(slider_view)

    return line_container, slider_view


def init_forecast_block(parent, label=None, bold=False, font=14):
    vl = init_container(parent, style="background-color: white;")
    # a vertical layout
    ql = QLabel()
    if label:
        if bold:
            ql.setStyleSheet("font: bold " + str(font) + "px;")
        elif not bold:
            ql.setStyleSheet("font: " + str(font) + "px;")

        # always in the center
        ql.setAlignment(QtCore.Qt.AlignCenter)

        ql.setText(label)
        vl.addWidget(ql)

    return vl, ql


def init_preparation_block(parent, text):
    vl = init_container(parent, style="background-color: white;")
    # a vertical layout
    ql1 = QLabel()
    ql1.setStyleSheet("font: 18px;")
    ql1.setAlignment(QtCore.Qt.AlignCenter)
    ql1.setText("You will be writing:  ")

    ql2 = QLabel()
    ql2.setStyleSheet("font: 14px;")
    ql2.setAlignment(QtCore.Qt.AlignCenter)

    ql2.setText(text)

    ql3 = QLabel()
    ql3.setStyleSheet("font: 18px;")
    ql3.setAlignment(QtCore.Qt.AlignCenter)
    ql3.setText("Press Enter To Continue")

    vl.addWidget(ql1)
    vl.addWidget(ql2)
    vl.addWidget(ql3)

    return vl


def init_instruction_text_block(parent, label_current=None, label_next=None, bold=False, font=14):
    vl = init_container(parent, style="background-color: white;")
    # a vertical layout
    ql1 = QLabel()
    ql1.setAlignment(QtCore.Qt.AlignCenter)
    ql1.setStyleSheet("font: 36px;")
    ql1.setText("Write")

    ql2 = QLabel()
    ql2.setAlignment(QtCore.Qt.AlignCenter)
    ql2.setStyleSheet("font: bold 32px;")
    ql2.setText(label_current)

    text3 = "... next"
    if label_next:
        text3 += " " + label_next
    elif not label_next:
        text3 += " NO NEXT"

    ql3 = QLabel()
    ql3.setAlignment(QtCore.Qt.AlignCenter)
    ql3.setStyleSheet("font: bold 32px;")
    ql3.setText(text3)

    return vl


def show_finished_box():
    # show finishing message
    msg = QMessageBox()
    msg.setIcon(QMessageBox.Information)
    msg.setText("Finished")
    msg.exec()


def init_locate_unit_block(parent, number=None, label_position="centertop", image_source=None):
    # contains a label(image) and another label(number)

    container = QWidget()
    parent.addWidget(container)
    layout = QHBoxLayout(container)

    image = QLabel()
    image.setAlignment(QtCore.Qt.AlignCenter)

    # if no image_source, use a place holder
    if not image_source:
        pixmap = QPixmap("resource/figures")
    else:
        pixmap = QPixmap(image_source)

    image.setPixmap(pixmap)
    container.resize(pixmap.width(), pixmap.height())

    # add the label
    label = QLabel()
    if not number:
        label.setText("0")
    else:
        label.setText(number)

    if label_position == "righttop":
        label.setAlignment(QtCore.Qt.AlignRight)
        label.setAlignment(QtCore.Qt.AlignTop)

    elif label_position == "lefttop":
        label.setAlignment(QtCore.Qt.AlignLeft)
        label.setAlignment(QtCore.Qt.AlignTop)

    layout.addWidget(label)


    return image
