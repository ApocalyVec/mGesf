from PyQt5.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QSlider, QLabel

from mGesf.main_page_tabs.gesture_tab import help_btn_action
from mGesf.main_page_tabs.gesture_tab.indexPen.prob_view import ProbViewWindow
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
        # 2. detection output block
        self.input_block = init_container(parent=self.main_page, vertical=True)
        self.detection_output_block = init_container(parent=self.main_page, vertical=False,
                                                     size=config.dtc_text_block_size)

        """
        adding to detection output block
        """
        self.lb_char_written = init_detection_text_block(self.detection_output_block)

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

        model_path_block, self.model_path_text = init_inputBox(self.text_box_block,
                                                               label=config.operation_model_path_label,
                                                               label_bold=False,
                                                               default_input=config.indexPen_modelPath_default)

        training_dir_block, self.training_dir_text = init_inputBox(self.text_box_block,
                                                                   label=config.trainingDataPath_label,
                                                                   label_bold=False,
                                                                   default_input=config.indexPen_trainingDataDir_default)

        # -------------------- fifth class --------------------
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

        self.prob_view_btn = init_button(parent=self.buttons_block,
                                         label=config.prob_view_btn_label,
                                         function=self.prob_view_btn_action)
        self.prob_view_win = ProbViewWindow(self)
        self.encoder = None  # idp classification categories

    def load_btn_action(self):
        try:
            self.encoder = retrieve_idp_encoder(self.training_dir_text.text())
        except FileNotFoundError as e:
            print('IndexPen Detection: ' + self.training_dir_text + ' does not exist.')

    def start_stop_btn_action(self):
        return

    def prob_view_btn_action(self):
        self.prob_view_win.show()
        print('hey')
