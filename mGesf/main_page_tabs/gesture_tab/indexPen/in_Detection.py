from PyQt5.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QSlider, QLabel

from mGesf.main_page_tabs.gesture_tab import help_btn_action
from utils.GUI_main_window import *
from utils.GUI_operation_tab import *
import config


class Detection(QWidget):
    def __init__(self):
        super().__init__()

        # -------------------- First class --------------------
        self.main_page = QHBoxLayout(self)
        self.setLayout(self.main_page)

        # -------------------- Second class --------------------
        # 1. input block
        # 2. detection image block
        self.input_block = init_container(parent=self.main_page, vertical=True)
        self.detection_image_block = init_container(parent=self.main_page, vertical=False)

        # -------------------- third class --------------------
        #   1. Input block
        #       1-1. text boxes
        #       1-2. Buttons
        self.text_box_block = init_container(self.input_block, vertical=True)
        self.buttons_block = init_container(self.input_block, vertical=False)

        # -------------------- fourth class --------------------
        #   1-1. text boxes
        #       1-1-1. model path
        #       1-1-2. training data dir

        self.model_path_block = init_inputBox(self.text_box_block,
                                              label=config.operation_model_path_label,
                                              label_bold=False,
                                              default_input='default: ' + config.indexPen_modelPath_default)

        self.training_dir_block = init_inputBox(self.text_box_block,
                                                label=config.trainingDataPath_label,
                                                label_bold=False,
                                                default_input='default: ' + config.indexPen_trainingDataDir_default)

        # -------------------- fourth class --------------------
        #   1-6. Buttons
        #       1-6-1. load button
        #       1-6-2. start/stop detection button
        #       1-6-3. Help button

        self.load_btn = init_button(parent=self.buttons_block,
                                    label=config.detection_load_btn_label,
                                    function=self.load_btn_action)
        self.start_stop_detection_btn = init_button(parent=self.buttons_block,
                                                    label=config.detection_start_btn_label,
                                                    function=self.start_stop_btn_action)

        self.help_btn = init_button(parent=self.buttons_block,
                                    label=config.help_btn_label,
                                    function=help_btn_action)
        self.show()

    def load_btn_action(self):
        return

    def start_stop_btn_action(self):
        return
