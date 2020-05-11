import webbrowser

from PyQt5.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QSlider, QLabel
from PyQt5.QtCore import Qt

from mGesf.main_page_tabs.gesture_tabs.Operation_tabs import help_btn_action
from utils.GUI_main_window import *
from utils.GUI_operation_tab import *
import config


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

        self.interval_last_block = init_slider_bar_box(self.input_block,
                                                       label=config.operation_interval_label,
                                                       interval=config.recording_interval_range)
        self.repeats_block = init_slider_bar_box(self.input_block, label=config.operation_repeats_label,
                                                 interval=config.recording_repeat_range)

        self.classes_block = init_input_box(self.input_block,
                                            label=config.operation_classes_label,
                                            label_bold=False,
                                            default_input=config.operation_classes_default)
        self.subject_name_block = init_input_box(self.input_block,
                                                 label=config.operation_subject_name_label,
                                                 label_bold=False,
                                                 default_input=config.operation_subject_name_default)
        self.training_dir_block = init_input_box(self.input_block,
                                                 label=config.operation_training_data_path_label,
                                                 label_bold=False,
                                                 default_input=config.operation_training_data_dir_default)

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
                                    label=config.test_btn_label,
                                    function=self.test_btn_action)

        self.recording_btn = init_button(parent=self.buttons_block,
                                         label=config.record_btn_label,
                                         function=self.recording_btn_action)

        self.help_btn = init_button(parent=self.buttons_block,
                                    label=config.help_btn_label,
                                    function=help_btn_action)

        self.show()

    def interrupt_btn_action(self):
        return

    def test_btn_action(self):
        return

    def recording_btn_action(self):
        return
