from PyQt5.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QTabWidget
from utils.GUI_main_window import init_container
from mGesf.main_page_tabs.gesture_tab.indexPen.idp_recording import IdpRecording
from mGesf.main_page_tabs.gesture_tab.indexPen.idp_train import IdpTrain
from mGesf.main_page_tabs.gesture_tab.indexPen.idp_detection import IdpDetection

import config


class IndexPen(QWidget):
    def __init__(self, *args, **kwargs):
        super().__init__()
        self.main_page = QHBoxLayout(self)

        self.operation_block = init_container(parent=self.main_page, vertical=False)

        # Initialize tab screen
        tabs = QTabWidget()
        tab1 = IdpRecording()
        tab2 = IdpTrain()
        tab3 = IdpDetection()

        tabs.addTab(tab1, config.operation_recording_label)
        tabs.addTab(tab2, config.operation_training_label)
        tabs.addTab(tab3, config.operation_detection_label)

        # Add tabs to widget
        self.operation_block.addWidget(tabs)

        self.setLayout(self.main_page)
        self.show()