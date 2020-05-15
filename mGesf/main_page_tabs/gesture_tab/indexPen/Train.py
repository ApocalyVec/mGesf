from PyQt5.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QSlider, QLabel
import pyqtgraph as pg
from utils.GUI_main_window import *
from utils.GUI_operation_tab import *
import config
import os
from utils.InformationPane import InformationPane
from utils.std_utils import Stream
import sys


class Train(QWidget):
    def __init__(self):
        super().__init__()

        self.isTraining = False
        self.is_modelDir_valid = False
        self.is_trainingDatalDir_valid = False
        self.is_constructorPath_valid = False

        # ==================  layouts  ==========================
        # -------------------- First class --------------------
        self.main_page = QHBoxLayout(self)
        self.setLayout(self.main_page)

        # -------------------- Second class --------------------
        # 1. input block (vertical)
        # 2. information
        self.input_block = init_container(parent=self.main_page, vertical=True)
        self.info_pane = InformationPane(parent=self.main_page, label=None)
        sys.stdout = Stream(newText=self.on_print)

        # -------------------- third class --------------------
        #   1. Input block
        #       1-1. Training Data Directory textbox
        #       1-2. Constructor Path textbox
        #       1-3. Model Directory textbox
        #       1-4. buttons

        self.trainingDataDir_block, self.trainingDataDir_textbox = init_inputBox(parent=self.input_block,
                                                                                 label="Training Data Directory",
                                                                                 default_input=config.thuMouse_TrainingDataDir_default)

        self.constructorPath_block, self.constructorPath_textbox = init_inputBox(parent=self.input_block,
                                                                                 label="Constructor Path",
                                                                                 default_input=config.thuMouse_constructorPath_default)

        self.editScript_btn = init_button(parent=self.constructorPath_block,
                                          label="edit\nscript",
                                          function=self.editScript_action)

        self.modelDir_block, self.modelDir_textbox = init_inputBox(parent=self.input_block,
                                                                   label="Model Directory",
                                                                   default_input=config.thuMouse_modelDir_default)

        self.buttons_block = init_container(parent=self.input_block,
                                            vertical=False)

        # -------------------- fourth class --------------------
        #       1-4. Buttons
        #           1-4-1. Count Sample
        #           1-4-2. Train
        #           1-4-3. Interrupt

        self.countSamples_btn = init_button(parent=self.buttons_block,
                                            label='Count Samples',
                                            function=self.countSamples_action)

        self.train_btn = init_button(parent=self.buttons_block,
                                     label="Train",
                                     function=self.train_action)

        self.interrupt_btn = init_button(parent=self.buttons_block,
                                         label=config.interrupt_btn_label,
                                         function=self.interrupt_btn_action)
        # ======================  inputs  ========================

        self.trainingDir = self.get_trainingData_dir()
        self.constructorPath = self.get_constructorPath()
        self.modelDir = self.get_model_dir()

    def countSamples_action(self):
        print("counting samples")
        return

    def train_action(self):
        print("training")
        return

    def editScript_action(self):
        print("editScript")
        return

    def interrupt_btn_action(self):
        # todo check 3 dirs valid
        self.isTraining = False
        return

    def get_trainingData_dir(self):
        _user_input = self.trainingDataDir_textbox.text()
        if not _user_input:
            _user_input = config.thuMouse_TrainingDataDir_default

        return _user_input

    def get_constructorPath(self):
        _user_input = self.constructorPath_textbox.text()
        if not _user_input:
            _user_input = config.thuMouse_constructorPath_default

        return _user_input

    def get_model_dir(self):
        _user_input = self.modelDir_textbox.text()
        if not _user_input:
            _user_input = config.thuMouse_modelDir_default

        return _user_input

    def check_dir_valid(self):

        if os.path.exists(self.trainingDir) and os.path.exists(self.modelDir) and os.path.exists(self.constructorPath):
            return True

        else:

            msg = QMessageBox()
            msg.setIcon(QMessageBox.Information)
            msg.setText("Exists invalid path/dir")
            msg.exec()

            if not os.path.exists(self.trainingDir):
                print("Invalid training data dir")

            if not os.path.exists(self.modelDir):
                print("Invalid model dir")

            if not os.path.exists(self.constructorPath):
                print("Invalid constructor path")

    def on_print(self, msg):
        self.info_pane.push(msg)