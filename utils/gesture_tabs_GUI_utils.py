from PyQt5.QtWidgets import QTabWidget

from mGesf.gesture_tabs.Operation_tabs import Detection_tab, Recording_tab, Train_tab
import config as config


def operation_tabs(parent):
    # Initialize tab screen
    parent.tabs = QTabWidget()
    parent.tab1 = Detection_tab.Detection_tab()
    parent.tab2 = Recording_tab.Recording_tab()
    parent.tab3 = Train_tab.Train_tab()

    parent.tabs.addTab(parent.tab1, config.gesture_label1)
    parent.tabs.addTab(parent.tab2, config.gesture_label2)
    parent.tabs.addTab(parent.tab3, config.gesture_label3)

    # Add tabs to widget
    parent.ITD_block.addWidget(parent.tabs)
    parent.setLayout(parent.ITD_block)

    parent.show()
