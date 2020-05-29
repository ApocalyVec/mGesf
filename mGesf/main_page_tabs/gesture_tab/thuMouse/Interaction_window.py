from PyQt5.QtWidgets import QMainWindow, qApp

from utils.GUI_operation_tab import *
import config
from mGesf.main_page_tabs.gesture_tab.thuMouse.Locate_Pane import Locate_Pane
import pyautogui as pag

# how many pixels does the cursor move in one step
step = 1


class Interaction_window(QMainWindow):
    def __init__(self, parent=None):
        """
        :param parent: The previous parent

        - The interaction window has both 1. a Lacate Pane and 2. a Follow Pane.
        - It shows only one Pane at a time.

        """
        super(Interaction_window, self).__init__(parent)

        self.resize(config.interaction_window_size[0], config.interaction_window_size[1])
        # a widget for resize() the Locate Pane
        # TODO THIS WIDGET SHOULD BE REMOVED IF WE CAN SET PANE LAYOUTS DIRECTLY TO THE WINDOW
        self.widget = QWidget()
        self.widget.setFixedSize(self.width(), self.height())
        self.layout().addWidget(self.widget)

        self.title = ''  # to be set later

        self.locate_layout = QGridLayout()
        self.follow_layout = QHBoxLayout()

        self.locate_pane = Locate_Pane(parent=self.locate_layout, rows=5, columns=5)
        self.follow_pane = None

        # --------------- LOGIC RELATED --------------------
        # state handler
        self.state = ['idle']       # resting when initialized

        # Keys allowed in the locate pane
        self.locate_key_map = [Qt.Key_Left,
                               Qt.Key_Up,
                               Qt.Key_Right,
                               Qt.Key_Down,
                               Qt.Key_Enter,
                               Qt.Key_Return,
                               Qt.Key_Space]

        # register for event filter
        qApp.installEventFilter(self)

        # for cursor control
        self.cursor_x, self.cursor_y = self.x(), self.y()

        """
        # A list of points the cursor went through
        """
        self.trace = []

    def open_locate_pane(self):
        self.setWindowTitle("Locate")
        self.widget.setLayout(self.locate_layout)

    def close_locate_pane(self):
        self.locate_layout.removeWidget(self.locate_pane)

    def open_follow_pane(self):
        self.setWindowTitle("Follow")
        self.follow_layout.addWidget(self.follow_pane)

    def close_follow_pane(self):
        self.follow_layout.removeWidget(self.follow_pane)

    def set_title(self, title):
        self.title = title

    def put_cursor_to_center(self):
        """Puts the cursor to the center of the canvas"""
        if 'locate' in self.state:
            print(str(self.cursor_x) + " " + str(self.cursor_y))
            pag.move(self.cursor_x, self.cursor_y)
            print('moved to' + str(self.cursor_x) + ' ' + str(self.cursor_y))

    def _cursor_left(self):
        self.cursor_x = self.cursor_x - step
        pag.move(self.cursor_x, self.cursor_y)

    def _cursor_right(self):
        self.cursor_x = self.cursor_x + step
        pag.move(self.cursor_x, self.cursor_y)

    def _cursor_up(self):
        self.cursor_y = self.cursor_y - step
        pag.move(self.cursor_x, self.cursor_y)

    def _cursor_down(self):
        self.cursor_y = self.cursor_y + step
        pag.move(self.cursor_x, self.cursor_y)

    def eventFilter(self, obj, event):
        """
        only allows key events and key presses
        :param obj:
        :param event:
        :return:
        """
        if event.type() == QtCore.QEvent.KeyPress:
            if event.key() in self.locate_key_map:
                # process key inputs
                key = event.key()
                self.key_logic(key)

                # check when started
                if 'ready' not in self.state:
                    # update the trace
                    # TODO: different pieces? only one?
                    self.trace.append(pag.position())

                    # check if target reached
                    self.check_target()

            return True
        return super().eventFilter(obj, event)

    def key_logic(self, key):
        """
        if 'locate':
            if 'idle':
                if 'ready' -> waiting for return/enter to be pressed
                else: [paused] -> waiting for space to be pressed

            else 'running':
                1. move by arrow keys
                2. if space pressed -> 'idle'

        :return:
        """
        if 'locate' in self.state:
            print(self.state)

            if 'idle' in self.state:
                if 'ready' in self.state:
                    # put the cursor to the origin of the window
                    # activate the locate pane
                    if key == QtCore.Qt.Key_Enter or key == QtCore.Qt.Key_Return:
                        print('started')
                        self.put_cursor_to_center()
                        self.locate_pane.activate()
                        self.state.remove('idle')
                        self.state.append('running')
                else:
                    if key == QtCore.Qt.Key_Space:
                        print('running')
                        self.state.remove('idle')
                        self.state.append('running')

            elif 'running' in self.state and key == QtCore.Qt.Key_Space:
                print('pause')
                self.state.remove('running')
                self.state.append('idle')

            if 'running' in self.state:
                if key == QtCore.Qt.Key_Up:
                    print('up')
                    self._cursor_up()

                elif key == QtCore.Qt.Key_Down:
                    print('down')
                    self._cursor_down()

                elif key == QtCore.Qt.Key_Left:
                    self._cursor_left()

                elif key == QtCore.Qt.Key_Right:
                    self._cursor_right()

    def check_target(self):
        """
        #   if the target is under mouse
        #   start a new round
        """
        if self.locate_pane.targets[self.locate_pane.activated_target].underMouse():
            self.locate_pane.random_switch()
