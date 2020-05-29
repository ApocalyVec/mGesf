from PyQt5.QtWidgets import QMainWindow

from utils.GUI_operation_tab import *
import config
import pyqtgraph as pg
from mGesf.main_page_tabs.gesture_tab.thuMouse.Locate_Pane import Locate_Pane


class Interaction_window(QMainWindow):
    def __init__(self, parent=None):
        super(Interaction_window, self).__init__(parent)
        self.cursor_x, self.cursor_y = self.x(), self.y()
        self.title = ''
        self.resize(config.interaction_window_size[0], config.interaction_window_size[1])
        self.widget = QWidget()
        self.widget.setFixedSize(self.width(), self.height())
        self.layout().addWidget(self.widget)

        self.locate_layout = QGridLayout()
        self.follow_layout = QHBoxLayout()

        self.locate_pane = Locate_Pane(parent=self.locate_layout, rows=5, columns=5)
        self.follow_pane = None
        self.state = 'pause'

    def open_locate_pane(self):
        self.setWindowTitle("Locate")
        self.widget.setLayout(self.locate_layout)

    def close_locate_pane(self):
        self.locate_layout.removeWidget(self.locate_pane)

    def open_follow_pane(self):
        self.setWindowTitle("Follow")
        # self.follow_layout.addWidget(self.follow_pane)

    def close_follow_pane(self):
        self.follow_layout.removeWidget(self.follow_pane)

    def set_title(self, title):
        self.title = title

    def put_cursor_to_center(self):
        """Puts the cursor to the center of the canvas"""
        if 'locate' in self.state:
            print(str(self.cursor_x) + " " + str(self.cursor_y))
            pag.move(self.cursor_x, self.cursor_y)
            print('moved to'+str(self.cursor_x)+' '+str(self.cursor_y))

    def _cursor_left(self):
        self.cursor_x = self.cursor_x + step
        pag.move(self.cursor_x, self.cursor_y)

    def _cursor_right(self):
        self.cursor_x = self.cursor_x - step
        pag.move(self.cursor_x, self.cursor_y)

    def _cursor_up(self):
        self.cursor_y = self.cursor_y - step
        pag.move(self.cursor_x, self.cursor_y)

    def _cursor_down(self):
        self.cursor_y = self.cursor_y + step
        pag.move(self.cursor_x, self.cursor_y)

    def keyPressEvent(self, key_event):

        if self.state == 'pause' and \
                key_event.key == QtCore.Qt.Key_Enter or key_event == QtCore.Qt.Key_Return:
            print('running')
            self.state = 'running'

        elif self.state == 'running' and key_event == QtCore.Qt.Key_Space:
            print('pause')
            self.state = 'pause'

        if self.state == 'running':
            if key_event.key() == QtCore.Qt.Key_Up:
                print('up')
                self._cursor_up()

            elif key_event.key() == QtCore.Qt.Key_Down:
                print('down')
                self._cursor_down()

            elif key_event.key() == QtCore.Qt.Key_Left:
                self._cursor_left()

            elif key_event.key() == QtCore.Qt.Key_Right:
                self._cursor_right()
