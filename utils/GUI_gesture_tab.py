from PyQt5.QtWidgets import QTabWidget

from mGesf.main_page_tabs.gesture_tab.indexPen.operation_tabs import Detection, Recording, Train
import config as config


def setup_operation_tabs(parent):
    # Initialize tab screen
    tabs = QTabWidget()
    tab1 = Recording.Recording_tab()
    tab2 = Train.Train_tab()
    tab3 = Detection.Detection_tab()

    tabs.addTab(tab1, config.operation_recording_label)
    tabs.addTab(tab2, config.operation_training_label)
    tabs.addTab(tab3, config.operation_detection_label)

    # Add tabs to widget
    parent.addWidget(tabs)

    return tabs
