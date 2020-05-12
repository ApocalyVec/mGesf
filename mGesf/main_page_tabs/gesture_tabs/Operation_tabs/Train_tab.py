from PyQt5.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QSlider, QLabel
from PyQt5.QtCore import Qt

from mGesf.main_page_tabs.gesture_tabs.Operation_tabs import help_btn_action
from utils.GUI_main_window import *
from utils.GUI_operation_tab import *
import config


class Train_tab(QWidget):
    def __init__(self):
        super().__init__()
        # -------------------- First class --------------------
        self.main_page = QHBoxLayout(self)
        self.setLayout(self.main_page)

        # -------------------- Second class --------------------
        # 1. input block
        # 2. information block
        self.input_block = init_container(parent=self.main_page, vertical=True)
        self.scrollArea, self.message = init_information_block(parent=self.main_page)

        # -------------------- third class --------------------
        #   1. Input block
        #       1-1. text boxes
        #       1-2. Buttons

        self.text_box_block = init_container(self.input_block, vertical=True)
        self.buttons_block = init_container(self.input_block, vertical=False)

        # -------------------- fourth class --------------------
        #   1-1. text boxes
        #       1-1-1. training data dir
        #       1-1-2. constructor path
        #       1-1-3. model dir

        self.training_dir_block = init_input_box(self.text_box_block,
                                                 label=config.operation_training_data_path_label,
                                                 label_bold=False,
                                                 default_input='default: ' + config.operation_training_data_dir_default)

        self.constructor_path_block = init_input_box(self.text_box_block,
                                                     label=config.operation_training_constructor_path_label,
                                                     default_input='default: ' + config.operation_constructor_path_default)

        self.model_dir_block = init_input_box(self.text_box_block,
                                              label=config.operation_model_dir_label,
                                              label_bold=False,
                                              default_input='default: ' + config.operation_constructor_path_default)

        # -------------------- fourth class --------------------
        #   1-6. Buttons
        #       1-6-1. count sample button
        #       1-6-2. train button
        #       1-6-3. Help button

        self.count_samples_btn = init_button(parent=self.buttons_block,
                                             label=config.count_sample_btn_label,
                                             function=self.count_samples_btn_action)
        self.train_btn = init_button(parent=self.buttons_block,
                                     label=config.train_btn_label,
                                     function=self.train_btn_action)

        self.help_btn = init_button(parent=self.buttons_block,
                                    label=config.help_btn_label,
                                    function=help_btn_action)
        self.show()

    def train_btn_action(self):
        return

    def count_samples_btn_action(self):
        return

    def interrupt_btn_action(self):
        return

    def test_btn_action(self):
        return

    def recording_btn_action(self):
        return
