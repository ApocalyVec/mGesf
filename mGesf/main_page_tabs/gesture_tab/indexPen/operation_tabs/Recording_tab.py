import os
import time

from PyQt5.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QSlider, QLabel, QMessageBox, QGraphicsScene, \
    QGraphicsView
from PyQt5 import QtCore
from PyQt5 import QtGui
from PyQt5.QtGui import QPainter, QBrush, QPen, QTransform
from PyQt5.QtWidgets import QApplication, QMainWindow

import sys
from mGesf.main_page_tabs.gesture_tab import help_btn_action
from utils.GUI_main_window import *
from utils.GUI_operation_tab import *
from utils.sound import *
import config
import pyqtgraph as pg


class Recording_tab(QWidget):
    def __init__(self):
        super().__init__()
        # -------------------- First class --------------------
        self.main_page = QHBoxLayout(self)
        self.setLayout(self.main_page)

        # -------------------- Second class --------------------
        # 1. input block
        # 2. recording block
        self.input_block = init_container(parent=self.main_page, vertical=True)
        self.recording_block = init_container(parent=self.main_page, vertical=False)

        # -------------------- third class --------------------
        #   1. Input block
        #       1-1. Interval last
        #       1-2. Repeats
        #       1-3. Classes
        #       1-4. Subject name
        #       1-5. Training data directory
        #       1-6. Buttons + help

        self.interval_last_block, self.interval_slider_view = init_slider_bar_box(self.input_block,
                                                                                  label=config.operation_interval_label,
                                                                                  interval=config.recording_interval_range)
        self.repeats_block, self.repeat_slider_view = init_slider_bar_box(self.input_block,
                                                                          label=config.operation_repeats_label,
                                                                          interval=config.recording_repeat_range)

        self.classes_block, self.classes_textbox = init_inputBox(self.input_block,
                                                                 label=config.operation_classes_label,
                                                                 label_bold=False,
                                                                 default_input=config.indexPen_classes_default)
        self.subject_name_block, self.subject_names_textbox = init_inputBox(self.input_block,
                                                                            label=config.operation_subject_name_label,
                                                                            label_bold=False,
                                                                            default_input=config.indexPen_subjectName_default)
        self.training_dir_block, self.training_dir_textbox = init_inputBox(self.input_block,
                                                                           label=config.operation_training_data_path_label,
                                                                           label_bold=False,
                                                                           default_input=config.indexPen_trainingDataDir_default)

        # -------------------- fourth class --------------------
        #   1-6. Buttons + help (horizontally distributed)
        #       1-6-1. Buttons
        #       1-6-2. Help (message box)

        self.buttons_block = init_container(self.input_block, vertical=False)
        self.help_block = init_container(self.input_block)

        # -------------------- fifth class --------------------
        #       1-6-1. Buttons
        #           1-6-1-1. Interrupt
        #           1-6-1-2. Start/end test
        #           1-6-1-3. Start Recording

        self.interrupt_btn = init_button(parent=self.buttons_block,
                                         label=config.interrupt_btn_label,
                                         function=self.interrupt_btn_action)

        self.test_btn = init_button(parent=self.buttons_block,
                                    label=config.test_btn_start_label,
                                    function=self.test_btn_action)

        self.recording_btn = init_button(parent=self.buttons_block,
                                         label=config.record_btn_start_label,
                                         function=self.recording_btn_action)

        self.help_btn = init_button(parent=self.buttons_block,
                                    label=config.help_btn_label,
                                    function=help_btn_action)

        # -------------------- third class --------------------
        #   1. Recording block (horizontal)
        #       1-1. circles block (vertical)
        #       1-2. message block (vertical)

        self.circles_block = init_container(parent=self.recording_block, vertical=True)
        self.circle_scene = QGraphicsScene()
        self.circle_view = QGraphicsView(self.circle_scene)
        self.x1, self.x2, self.x3, self.x4, self.y1, self.y2, self.y3, self.y4, self.circle_scene_width, \
        self.circle_scene_height = self.setup_canvas()
        self.paint(first_circle_colored=False)

        self.message_block = init_container(parent=self.recording_block, vertical=True)

        self.show()

        self.interval = self.interval_slider_view.slider.value()
        self.repeats = self.repeat_slider_view.slider.value()
        self.classes = self.get_classes()
        self.subject_name = self.get_subject_name()
        self.training_dir = self.get_training_data_dir()
        self.is_dir_valid = False

        # timer
        # indicator for circle colors
        self.start_time = time.time()
        self.tick_count = 0
        self.counter = 1
        self.reset_recording()
        self.timer = QtCore.QTimer()
        self.timer.setInterval(1000)
        self.timer.timeout.connect(self.ticks)

        # init sound device with playing some test sound
        dah()

    def reset_recording(self):
        self.start_time = time.time()
        self.tick_count = 0
        self.counter = 1

    @pg.QtCore.pyqtSlot()
    def ticks(self):
        """
        ticks every 'refresh' milliseconds
        """
        self.tick_count += 1
        if self.counter == 1:
            dah()
        else:
            dih()

        self.repaint(circle=self.counter)
        self.counter = self.counter + 1 if self.counter < 4 else 1  # TODO use 'interval lasts' instead of hard coded 4

    def setup_canvas(self):

        self.circles_block.addWidget(self.circle_view)
        self.circle_view.resize(config.WINDOW_WIDTH / 9, config.WINDOW_HEIGHT / 3)
        position = self.circle_view.pos()

        self.circle_scene.setSceneRect(position.x(), position.y(), self.circle_view.width(), self.circle_view.height())

        # size of the scene
        width = self.circle_scene.width()
        height = self.circle_scene.height()

        # positions of circles
        x1, y1 = position.x() + width / 3, position.y()
        x2, y2 = position.x() + width / 3, position.y() + 3 * (height / 10)
        x3, y3 = position.x() + width / 3, position.y() + 6 * (height / 10)
        x4, y4 = position.x() + width / 3, position.y() + 9 * (height / 10)

        return x1, x2, x3, x4, y1, y2, y3, y4, width, height

    def repaint(self, circle=1):

        x = self.x1
        y = self.y1

        if circle == 1:
            # remove all current circles
            for item in self.circle_scene.items():
                self.circle_scene.removeItem(item)
            # paint them light gray
            self.paint()

        else:
            if circle == 2:
                x = self.x2
                y = self.y2
            elif circle == 3:
                x = self.x3
                y = self.y3
            elif circle == 4:
                x = self.x4
                y = self.y4

        # locate the circle
        circle = self.circle_scene.itemAt(x, y, QTransform())
        # remove the original circle
        self.circle_scene.removeItem(circle)

        # repaint a blue one
        pen = QPen(Qt.black, 5, Qt.SolidLine)
        brush = QBrush(Qt.blue, Qt.SolidPattern)
        self.circle_scene.addEllipse(x, y,
                                     self.circle_scene_width / 3, self.circle_scene_width / 3,
                                     pen, brush)

    def paint(self, first_circle_colored=False):

        pen = QPen(Qt.black, 5, Qt.SolidLine)
        brush = QBrush(Qt.lightGray, Qt.SolidPattern)

        self.circle_scene.addEllipse(self.x2, self.y2,
                                     self.circle_scene_width / 3, self.circle_scene_width / 3,
                                     pen, brush)
        self.circle_scene.addEllipse(self.x3, self.y3,
                                     self.circle_scene_width / 3, self.circle_scene_width / 3,
                                     pen, brush)
        self.circle_scene.addEllipse(self.x4, self.y4,
                                     self.circle_scene_width / 3, self.circle_scene_width / 3,
                                     pen, brush)

        if first_circle_colored:
            brush = QBrush(Qt.blue, Qt.SolidPattern)

        self.circle_scene.addEllipse(self.x1, self.y1,
                                     self.circle_scene_width / 3, self.circle_scene_width / 3,
                                     pen, brush)

    def update_inputs(self):
        self.interval = self.interval_slider_view.slider.value()
        self.repeats = self.repeat_slider_view.slider.value()
        self.classes = self.get_classes()
        self.subject_name = self.get_subject_name()
        self.training_dir = self.get_training_data_dir()

    def get_training_data_dir(self):
        _user_input = self.training_dir_textbox.text()
        if not _user_input:
            _user_input = config.operation_training_data_dir_default

        return _user_input

    def get_subject_name(self):
        _user_input = self.subject_names_textbox.text()
        if not _user_input:
            _user_input = config.operation_subject_name_default
        return _user_input

    def get_classes(self):
        _user_input = self.classes_textbox.text()
        if not _user_input:
            _user_input = config.operation_classes_default

        # change to a list
        classes = _user_input.split(" ")
        return classes

    def interrupt_btn_action(self):
        self.timer.stop()
        record_duration = time.time() - self.start_time
        print('Recording interrupted. Time wasted: ' + str(record_duration) + ' sec')
        return

    def test_btn_action(self):
        self.update_inputs()
        # start the timer
        self.timer.start()

        return

    def recording_btn_action(self):
        self.update_inputs()
        self.is_dir_valid = self.check_dir_valid()
        if self.is_dir_valid:
            self.reset_recording()
            self.timer.start()

        return

    def check_dir_valid(self):
        if os.path.exists(self.training_dir):
            return True
        else:
            msg = QMessageBox()
            msg.setIcon(QMessageBox.Information)
            msg.setText(config.datapath_invalid_message)
            msg.exec()
            return False
