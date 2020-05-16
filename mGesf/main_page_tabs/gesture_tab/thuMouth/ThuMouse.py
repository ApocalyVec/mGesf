from PyQt5.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QTabWidget
from utils.GUI_main_window import init_container
from mGesf.main_page_tabs.gesture_tab.thuMouth.thu_Recording import Recording
from mGesf.main_page_tabs.gesture_tab.thuMouth.thu_Train import Train
from mGesf.main_page_tabs.gesture_tab.thuMouth.thu_Detection import Detection
import config


class ThuMouth(QWidget):
    def __init__(self):
        super().__init__()
        self.main_page = QHBoxLayout(self)

        self.operation_block = init_container(parent=self.main_page, vertical=False)
        # Initialize tab screen
        tabs = QTabWidget()
        tab1 = Recording()
        tab2 = Train()
        tab3 = Detection()

        tabs.addTab(tab1, config.operation_recording_label)
        tabs.addTab(tab2, config.operation_training_label)
        tabs.addTab(tab3, config.operation_detection_label)

        # Add tabs to widget
        self.operation_block.addWidget(tabs)
        self.setLayout(self.main_page)

        self.show()
