import pickle

from PyQt5 import QtGui, QtCore
from PyQt5.QtWidgets import QLabel, QHBoxLayout, QSlider, QMessageBox, QWidget, QVBoxLayout, QScrollArea, QGroupBox, \
    QFormLayout
from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import QLabel, QHBoxLayout, QSlider
import config
from mGesf.utils import camel_case_split
from utils.GUI_main_window import init_container, init_view
from utils.labeled_Slider import LabeledSlider
from PyQt5.QtGui import QIcon, QPixmap
import os
import pyqtgraph as pg


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


def init_countdown_block(parent, label=None, bold=False, font=14):
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


def init_instruction_text_block(parent, bold=False, font=14):
    vl = init_container(parent, style="background-color: white;")
    # a vertical layout
    lb_write = QLabel()
    lb_write.setAlignment(QtCore.Qt.AlignCenter)
    lb_write.setStyleSheet("font: 32px;")
    lb_write.setText("Write")
    vl.addWidget(lb_write)

    lb_char_to_write = QLabel()
    lb_char_to_write.setAlignment(QtCore.Qt.AlignCenter)
    lb_char_to_write.setStyleSheet("font: bold 56px;")
    lb_char_to_write.setText('...')
    vl.addWidget(lb_char_to_write)

    lb_char_next = QLabel()
    lb_char_next.setAlignment(QtCore.Qt.AlignCenter)
    lb_char_next.setStyleSheet("font: italic 24px;")
    lb_char_next.setText(config.instruction_next_text + '...')
    vl.addWidget(lb_char_next)

    return lb_char_to_write, lb_char_next


def init_detection_text_block(parent, bold=False, font=14):
    vl = init_container(parent, style="background-color: white;")
    # a vertical layout
    lb_write = QLabel()
    lb_write.setAlignment(QtCore.Qt.AlignCenter)
    lb_write.setStyleSheet("font: 32px;")
    lb_write.setText("You wrote")
    vl.addWidget(lb_write)

    lb_char_written = QLabel()
    lb_char_written.setAlignment(QtCore.Qt.AlignCenter)
    lb_char_written.setStyleSheet("font: bold 56px;")
    lb_char_written.setText('... nothing')
    vl.addWidget(lb_char_written)

    return lb_char_written


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


def init_scroll_area(parent, label, size=None):
    gb = QGroupBox(label)
    form = QFormLayout()
    gb.setLayout(form)

    scroll = QScrollArea()
    scroll.setWidget(gb)
    scroll.setWidgetResizable(True)
    scroll.resize(size[0], size[1])
    parent.addWidget(scroll)
    return form, scroll


def retrieve_idp_encoder(train_dir: str):
    """
    files in the train_dir are name <idp>-<classes>-rpt<repetition times>
    the classes should be in camel case
    resolve classes using the one-hot encoder saved from the training
    @param train_dir:
    """
    return pickle.load(open(os.path.join(train_dir, 'idp_encoder.pickle'), 'rb'))
    # classes = []
    # for fn in os.listdir(train_dir):
    #     seps = fn.split('-')
    #     if len(seps) == 3 and seps[0] == 'idp':  # identify the data files
    #         classes += camel_case_split(seps[1])
    # return classes


def clear_layout(layout):
    for i in reversed(range(layout.count())):
        layout.itemAt(i).widget().setParent(None)


def circular_sampling(sample, point, timestep):
    rtn = sample[-(timestep - 1):]
    rtn.append([point])  # expand the channel dimension
    return rtn


def create_plot_widget(x_lim=None, y_lim=None):
    plot_widget = pg.PlotWidget()
    if x_lim:
        plot_widget.setXRange(*x_lim)
    if y_lim:
        plot_widget.setYRange(*y_lim)
    plot = plot_widget.plot([], [], pen=pg.mkPen(color=(0, 0, 255)))
    return plot_widget, plot
