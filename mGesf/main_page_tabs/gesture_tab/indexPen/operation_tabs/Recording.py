import os
from PyQt5.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QSlider, QLabel, QMessageBox, QGraphicsScene, \
    QGraphicsView
from PyQt5 import QtCore
from PyQt5 import QtGui
from PyQt5.QtGui import QPainter, QBrush, QPen, QTransform
from PyQt5.QtWidgets import QApplication, QMainWindow
import sys
from mGesf.main_page_tabs.gesture_tab import help_btn_action, calc_set
from mGesf.main_page_tabs import ReturnKeyDetectionWidget

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
        # 1. input block (vertical)
        # 2. instruction block (horizontal)
        self.input_block = init_container(parent=self.main_page,
                                          vertical=True,
                                          style=None)

        self.instruction_block = init_container(parent=self.main_page,
                                                vertical=False,
                                                style="background-color: white;")
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

        self.classes_block, self.classes_textbox = init_input_box(self.input_block,
                                                                  label=config.operation_classes_label,
                                                                  label_bold=False,
                                                                  default_input='default: ' + config.operation_classes_default)
        self.subject_name_block, self.subject_names_textbox = init_input_box(self.input_block,
                                                                             label=config.operation_subject_name_label,
                                                                             label_bold=False,
                                                                             default_input='default: ' + config.operation_subject_name_default)
        self.training_dir_block, self.training_dir_textbox = init_input_box(self.input_block,
                                                                            label=config.operation_training_data_path_label,
                                                                            label_bold=False,
                                                                            default_input='default: ' + config.operation_training_data_dir_default)

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
        #   1. Instruction block (horizontal)
        #       1-1. circles block (vertical)
        #       1-2. text block (vertical)

        self.circles_block = init_container(parent=self.instruction_block, vertical=True)
        self.text_block = init_container(parent=self.instruction_block, vertical=True)
        # -------------------- fourth class --------------------
        #       1-1. circles block (vertical)
        #           1-1-1. circles_view
        #               1-1-1-1. circles_scene
        #                   1-1-1-1. 4 circles drawn to the scene

        self.circle_scene = QGraphicsScene()
        self.circle_view = QGraphicsView(self.circle_scene)
        # positions of the four circles
        # stored for redraw
        self.x1, self.x2, self.x3, self.x4, self.y1, self.y2, self.y3, self.y4, self.circle_scene_width, \
        self.circle_scene_height = self.setup_canvas()
        # show the circles
        self.paint(first_circle_colored=False)

        # -------------------- fourth class --------------------
        #       1-2. text block (vertical)
        #           ------- preparation -------
        #           1-1-1. "You will be writing:  "
        #                  character_set ::= A sequence of text the user will write
        #                  "Press Enter To Continue"
        #
        #           ------- forecast ------
        #           1-1-1. Label1: Forecast
        #
        #           ------- record ------
        #           1-1-1. Label1: "Write"
        #                  character to write
        #                  "...next" + next character to write

        # will be initialized when the test button or the recording button is pressed
        self.preparation_block = None
        # will be initialized when the user presses enter/return after preparation
        self.forecast_block = None
        # will be initialized when the forecast animation is over
        self.instruction_text_block = None

        self.show()

        # ============================= others ==========================

        # get input values
        self.interval = self.interval_slider_view.slider.value()
        self.repeat_times = self.repeat_slider_view.slider.value()
        # stored in a list
        self.classes = self.get_classes()
        self.subject_name = self.get_subject_name()
        self.training_dir = self.get_training_data_dir()
        self.character_set = calc_set(self.classes, self.repeat_times)


        # =========================== timers =============================
        # timer 1
        self.timer = QtCore.QTimer()
        self.timer.setInterval(self.interval * 1000)
        self.timer.timeout.connect(self.ticks)

        # timer 2
        # an indicator for the recording forecast, the ready go part
        self.forecast_timer = QtCore.QTimer()
        self.forecast_timer.setInterval(config.forecast_interval * 1000)
        self.forecast_timer.timeout.connect(self.show_forecast_animation)


        # ======================= indicators, counters ==========================
        # tracking if the user pressed the return key to start recording
        self.is_prepared = False
        self.is_return_pressed = False

        self.is_dir_valid = False

        self.is_testing = False
        self.is_recording = False

        self.current_forecast_num = 0
        # an indicator for inputting gestures
        self.tempo_counter = 1
        # count indices of current and next character to take in
        self.character_counter = 0


    @pg.QtCore.pyqtSlot()
    def show_forecast_animation(self):
        # after the text block shows all forecast texts, stop updating, do nothing and return
        if self.current_forecast_num == len(config.forecast_animation_text):
            self.current_forecast_num += 1
            return

        # wait for a given interval and start the input timer
        elif self.current_forecast_num > len(config.forecast_animation_text):
            # end the forecast timer
            self.forecast_timer.stop()
            # delete the forecast text block
            self.forecast_block = None
            self.timer.start()

        # otherwise show the text
        else:
            self.forecast_label.setText(config.forecast_animation_text[self.current_forecast_num])
            print(config.forecast_animation_text[self.current_forecast_num])
            # update
            self.current_forecast_num += 1

        return

    @pg.QtCore.pyqtSlot()
    def ticks(self):
        """
        ticks every 'refresh' milliseconds
        """

        # tempo: dah, dih, dih,dih
        if self.tempo_counter == 1:
            # dah()
            dih()
        else:
            dih()

        # repaint circles
        self.repaint(circle=self.tempo_counter)

        self.tempo_counter += 1
        if self.tempo_counter > 4:
            self.tempo_counter = 1

        # instruction text
        if self.character_counter < len(self.character_set):
            # undraw the previous text block
            self.instruction_text_block = None

            # draw a new one
            current_character = self.character_set[self.character_counter]
            next_character = self.character_set[self.character_counter+1]
            self.instruction_text_block =init_instruction_text_block(label_current=current_character,
                                                                     label_next=next_character)
        # finish a recording loop
        else:
            self.reset()
            show_finished_box()

        # update to next character
        self.character_counter += 1

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
        if circle:
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
        self.repeat_times = self.repeat_slider_view.slider.value()
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
        self.is_testing = False
        self.is_recording = False

        if self.forecast_timer.isActive():
            self.forecast_timer.stop()

        if self.timer.isActive():
            self.timer.stop()

        return

    def test_btn_action(self):


        if self.is_testing:
            # end testing
            self.reset()


        elif not self.is_testing:
            # start recording
            self.is_testing = True
            self.test_btn.setText(config.test_btn_start_label)

            self.update_inputs()
            self.prepare()
            if self.is_return_pressed:
                # destroy the preparation text block
                # create the forecast block
                self.forecast_block = init_forecast_block(self.text_block, label="   ", font=36, bold=True)

                # start the timer
                self.forecast_timer.start()

        return

    def recording_btn_action(self):

        if self.is_recording:
            self.reset()

        # if not recording yet
        elif not self.is_recording:

            # try starting recording
            # check the data path first
            self.is_dir_valid = self.check_dir_valid()

            if self.is_dir_valid:
                # if valid data path, show preparation page

                # start recording
                self.is_recording = True
                self.recording_btn.setText(config.record_btn_end_label)
                self.forecast_timer.start()

                # TODO change this to the information box
                msg = QMessageBox()
                msg.setIcon(QMessageBox.Information)
                msg.setText("Recording")
                msg.exec()

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


    def prepare(self):
        print("here")

        # print preparation direction to textbox
        self.preparation_block = init_preparation_block(parent=self.text_block, text=self.character_set)

        #create a return key detection widget to the instruction box
        key_detector = ReturnKeyDetectionWidget()
        self.text_block.addWidget(key_detector)
        key_detector.keyPressed.connect(self.on_key)
        print(self.is_return_pressed)

        # create a

    def set_KeyPressed_True(self):
        self.is_return_pressed = True

    def set_KeyPressed_False(self):
        self.is_return_pressed = False

    def on_key(self, key, tab):
        # test for a specific key
        if key == QtCore.Qt.Key_Return or key == QtCore.Qt.Key_Enter:
            tab.set_KeyPressed_True()

        else:
            tab.set_KeyPressed_False()

    def reset(self):

        # stop timers
        if self.timer.isActive():
            self.timer.stop()
        if self.forecast_timer.isActive():
            self.forecast_timer.stop()

        # clear the instruction
        if self.preparation_block:
            self.preparation_block = None
        if self.forecast_block:
            self.forecast_block = None
        if self.instruction_text_block:
            self.instruction_text_block = None

        # reset circles
        for item in self.circle_scene.items():
            self.circle_scene.removeItem(item)
        self.paint()

        print("here")

        # reset indicators
        self.is_testing = False
        self.is_recording = False
        self.tempo_counter = 1
        self.character_counter = 0

        self.test_btn.setText(config.test_btn_start_label)
        self.recording_btn.setText(config.record_btn_start_label)


