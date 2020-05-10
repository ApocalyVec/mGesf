from PyQt5.QtWidgets import QTabWidget

from mGesf.main_page_tabs.gesture_tabs.Operation_tabs import Recording_tab, Detection_tab, Train_tab
import config as config


def setup_operation_tabs(parent):
    # Initialize tab screen
    tabs = QTabWidget()
    tab1 = Detection_tab.Detection_tab()
    tab2 = Recording_tab.Recording_tab()
    tab3 = Train_tab.Train_tab()

    tabs.addTab(tab1, config.operation_label1)
    tabs.addTab(tab2, config.operation_label2)
    tabs.addTab(tab3, config.operation_label3)

    # Add tabs to widget
    parent.addWidget(tabs)

    return  tabs
