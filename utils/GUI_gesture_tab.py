from PyQt5.QtWidgets import QTabWidget

from mGesf.main_page_tabs.gesture_tabs.Operation_tabs import Recording_tab, Detection_tab, Train_tab
import config as config


def setup_operation_tabs(parent):
    # Initialize tab screen
    tabs = QTabWidget()
    tab1 = Recording_tab.Recording_tab()
    tab2 = Train_tab.Train_tab()
    tab3 = Detection_tab.Detection_tab()

    tabs.addTab(tab1, config.operation_recording_label)
    tabs.addTab(tab2, config.operation_training_label)
    tabs.addTab(tab3, config.operation_detection_label)

    # Add tabs to widget
    parent.addWidget(tabs)

    return tabs
