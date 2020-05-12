from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import QLabel, QHBoxLayout, QSlider

from utils.GUI_main_window import init_container


def init_slider_bar_box(parent, label=None, interval=5):
    line_container = init_container(parent=parent, vertical=False)

    if label:
        ql = QLabel()
        ql.setStyleSheet("font: 14px;")
        ql.setText(label)
        line_container.addWidget(ql)

    # slider with interval
    slider = QSlider(Qt.Horizontal)
    slider.setFocusPolicy(Qt.StrongFocus)
    slider.setTickPosition(QSlider.TicksBothSides)
    slider.setTickInterval(interval)
    slider.setSingleStep(1)

    line_container.addWidget(slider)

    return line_container, slider
