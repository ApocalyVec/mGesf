from PyQt5.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QTabWidget
from mGesf.gesture_tabs.Operation_tabs import Detection, Recording_tab, Train_tab
import mGesf.config as config

class DesktopFinger_tab(QWidget):
    def __init__(self):
        super().__init__()
        self.layout = QHBoxLayout(self)

