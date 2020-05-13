from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import QLabel, QHBoxLayout, QSlider

from utils.GUI_main_window import init_container
from utils.labeled_Slider import LabeledSlider

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
