from PyQt5.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QTabWidget
from utils.GUI_main_window import init_container
from mGesf.main_page_tabs.gesture_tab.indexPen.idp_recording import IdpRecording
from mGesf.main_page_tabs.gesture_tab.indexPen.idp_train import IdpTrain
from mGesf.main_page_tabs.gesture_tab.indexPen.idp_detection import IdpDetection

import config


class IndexPen(QWidget):
    def __init__(self, record_signal, mmw_signal, *args, **kwargs):
        super().__init__()
        self.main_page = QHBoxLayout(self)

        self.operation_block = init_container(parent=self.main_page, vertical=False)

        # Initialize tab screen
        tabs = QTabWidget()
        self.tab_recording = IdpRecording(record_signal)
        self.tab_training = IdpTrain()
        self.tab_detection = IdpDetection(mmw_signal)

        tabs.addTab(self.tab_recording, config.operation_recording_label)
        tabs.addTab(self.tab_training, config.operation_training_label)
        tabs.addTab(self.tab_detection, config.operation_detection_label)

        # Add tabs to main_widget
        self.operation_block.addWidget(tabs)

        self.setLayout(self.main_page)
        self.show()

    def get_record_data_path(self):
        return self.tab_recording.get_data_path()