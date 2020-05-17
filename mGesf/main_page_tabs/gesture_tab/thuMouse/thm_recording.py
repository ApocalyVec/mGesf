from PyQt5.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QSlider, QLabel
import pyqtgraph as pg
from utils.GUI_main_window import *
from utils.GUI_operation_tab import *
import config
import os

class Recording(QWidget):
    def __init__(self):
        super().__init__()

        self.willFollow = False
        self.willLocate = False
        self.isFollowing = False
        self.isLocating = False
        self.is_testing = False
        self.is_recording = False
        self.is_dir_valid = False

        # for checking only one box
        self._toggle = None

        # the timer
        self.timer = QtCore.QTimer()
        self.timer.setInterval(1000)
        self.timer.timeout.connect(self.ticks)

        # ==================  layouts  ==========================
        # -------------------- First class --------------------
        self.main_page = QHBoxLayout(self)
        self.setLayout(self.main_page)

        # -------------------- Second class --------------------
        # 1. input block (vertical)
        # 2. recording block
        self.input_block = init_container(parent=self.main_page, vertical=True)
        self.recording_block = init_container(parent=self.main_page, vertical=False)

        # -------------------- third class --------------------
        #   1. Input block
        #       1-1. task selection box (horizontal)
        #       1-2. subject name textbox
        #       1-3. training dir textbox
        #       1-4. buttons
        self.taskSelection_block = init_container(parent=self.input_block,
                                                  label="Task Selection",
                                                  vertical=False)

        self.subjectName_block, self.subjectName_textbox = init_inputBox(parent=self.input_block,
                                                                         label="Subject Name",
                                                                         default_input=config.thuMouse_subjectName_default)

        self.trainingDataDir_block, self.trainingDataDir_textbox = init_inputBox(parent=self.input_block,
                                                                                 label="Training Data Directory",
                                                                                 default_input=config.thuMouse_TrainingDataDir_default)

        self.buttons_block = init_container(parent=self.input_block,
                                            vertical=False)

        # -------------------- fourth class --------------------
        #   1-1. task selection box (horizontal)
        #       1-1-1. follow checkbox
        #       1-1-2. locate checkbox

        self.follow_checkbox = init_checkBox(parent=self.taskSelection_block,
                                             label="follow",
                                             function=self.follow_checkBox_action)

        self.locate_checkbox = init_checkBox(parent=self.taskSelection_block,
                                             label="locate",
                                             function=self.locate_checkBox_action)

        # -------------------- fourth class --------------------
        #       1-4. Buttons
        #           1-4-1. Interrupt
        #           1-4-2. Start/end test
        #           1-4-3. Start Recording

        self.interrupt_btn = init_button(parent=self.buttons_block,
                                         label=config.interrupt_btn_label,
                                         function=self.interrupt_btn_action)

        self.test_btn = init_button(parent=self.buttons_block,
                                    label=config.test_btn_start_label,
                                    function=self.test_btn_action)

        self.recording_btn = init_button(parent=self.buttons_block,
                                         label=config.record_btn_start_label,
                                         function=self.recording_btn_action)


        # -------------------- third class --------------------
        #   1. recording block
        #       1-1. Follow canvas
        #       -------------------
        #       1-1. locate canvas (vertical)
        #           1-1-1. circle1 + circle3 (vertical)
        #           1-1-2. circle2 + circle4 (vertical)

        # locate canvas
        self.locate_canvas = init_container(parent=self.recording_block,
                                            vertical=False)

        self.locate_canvas1_3 = init_container(parent=self.locate_canvas,
                                               vertical=True)

        self.locate_canvas2_4 = init_container(parent=self.locate_canvas,
                                               vertical=True)

        # initialized when the locate box is selected and start testing/recording
        self.locate_1 = None
        self.locate_2 = None
        self.locate_3 = None
        self.locate_4 = None

        self.subject_name = self.get_subject_name()
        self.training_dir = self.get_training_data_dir()


    @pg.QtCore.pyqtSlot()
    def ticks(self):
        """
        ticks every 'refresh' milliseconds
        """
        print("tick")

    def reset(self):
        self.isFollowing = False
        self.isLocating = False
        self.is_testing = False
        self.is_recording = False
        self.is_dir_valid = False
        self.timer.stop()
        self.test_btn.setText(config.test_btn_start_label)
        self.recording_btn.setText(config.record_btn_start_label)

    def interrupt_btn_action(self):
        self.is_testing = False
        self.is_recording = False
        self.timer.stop()

        return

    def test_btn_action(self):

        if self.is_testing:
            # end testing
            self.reset()

        elif not self.is_testing:
            # start recording
            print("Testing")

            self.is_testing = True
            self.timer.start()
            self.test_btn.setText(config.test_btn_end_label)
            if self.willLocate:
                self.locate_1 = init_locate_unit_block(parent=self.locate_canvas1_3,
                                                       number="1",
                                                       label_position="righttop",
                                                       image_source="resource/figures/circle_off")

                self.locate_3 = init_locate_unit_block(parent=self.locate_canvas1_3,
                                                       number="3",
                                                       label_position="righttop",
                                                       image_source="resource/figures/circle_off")

                self.locate_2 = init_locate_unit_block(parent=self.locate_canvas2_4,
                                                       number="1",
                                                       label_position="lefttop",
                                                       image_source="resource/figures/circle_off")

                self.locate_4 = init_locate_unit_block(parent=self.locate_canvas2_4,
                                                       number="3",
                                                       label_position="lefttop",
                                                       image_source="resource/figures/circle_off")

        return

    def recording_btn_action(self):

        if self.is_recording:
            self.reset()

        # if not recording yet
        elif not self.is_recording:

            self.update_inputs()
            # try starting recording
            # check the data path first
            self.is_dir_valid = self.check_dir_valid()

            if self.is_dir_valid:
                # if valid data path, show preparation page

                print("Recording")
                # start recording
                self.is_recording = True
                self.recording_btn.setText(config.record_btn_end_label)

                # TODO change this to the information box
                msg = QMessageBox()
                msg.setIcon(QMessageBox.Information)
                msg.setText("Recording")
                msg.exec()

        return

    def follow_checkBox_action(self):
        if self.follow_checkbox.isChecked():
            self.willFollow = True
            self._toggle = True
            self.locate_checkbox.setChecked(not self._toggle)
        else:
            self.willFollow = False
            self._toggle = not self._toggle

        '''
        print("follow?")
        print(self.willFollow)
        print("locate?")
        print(self.willLocate)
        '''
        return

    def locate_checkBox_action(self):
        if self.locate_checkbox.isChecked():
            self.willLocate = True
            self._toggle = True
            self.follow_checkbox.setChecked(not self._toggle)

        else:
            self.willLocate = False
            self._toggle = not self._toggle

        '''
        print("follow?")
        print(self.willFollow)
        print("locate?")
        print(self.willLocate)
        '''
        return

    def get_training_data_dir(self):
        _user_input = self.trainingDataDir_textbox.text()
        if not _user_input:
            _user_input = config.thuMouse_TrainingDataDir_default

        return _user_input

    def get_subject_name(self):
        _user_input = self.subjectName_textbox.text()
        if not _user_input:
            _user_input = config.thuMouse_subjectName_default
        return _user_input

    def check_dir_valid(self):
        if os.path.exists(self.training_dir):
            return True
        else:
            msg = QMessageBox()
            msg.setIcon(QMessageBox.Information)
            msg.setText(config.datapath_invalid_message)
            msg.exec()
            return False
