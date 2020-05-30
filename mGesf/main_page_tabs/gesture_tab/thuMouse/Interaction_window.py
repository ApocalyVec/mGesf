import time

import pyautogui
from PyQt5.QtWidgets import QMainWindow, qApp
from utils.GUI_operation_tab import *
import config
from mGesf.main_page_tabs.gesture_tab.thuMouse.LocatePane import LocatePane
from mGesf.main_page_tabs.gesture_tab.thuMouse.FollowPane import FollowPane

import pyautogui as pag

# how many pixels does the cursor move in one step
step = 5


def _cursor_up():
    pyautogui.moveRel(0, -step)


def _cursor_down():
    pyautogui.moveRel(0, step)


def _cursor_right():
    pyautogui.moveRel(step, 0)


def _cursor_left():
    pyautogui.moveRel(-step, 0)


def arrow_key_event(key):
    if key == QtCore.Qt.Key_Up:
        print('up')
        _cursor_up()
        return
    elif key == QtCore.Qt.Key_Down:
        print('down')
        _cursor_down()
        return
    elif key == QtCore.Qt.Key_Left:
        print('left')
        _cursor_left()
        return
    elif key == QtCore.Qt.Key_Right:
        print('right')
        _cursor_right()
        return

    return


class Interaction_window(QMainWindow):
    def __init__(self, parent=None, repeat_times=config.thuMouse_repeatTimes_default):
        """
        :param parent: The previous parent

        - The interaction window has both 1. a Lacate Pane and 2. a Follow Pane.
        - It shows only one Pane at a time.

        """
        super(Interaction_window, self).__init__(parent)
        screen_size = get_screen_size()

        self.parent = parent
        self.resize(config.interaction_window_size[0], config.interaction_window_size[1])
        self.main_widget = QWidget()
        self.main_widget.setFixedSize(*screen_size)
        self.setCentralWidget(self.main_widget)
        size_policy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Expanding)
        self.main_widget.setSizePolicy(size_policy)
        self.layout().addWidget(self.main_widget)

        self.locate_layout = QGridLayout()
        self.locate_layout.setGeometry(self.main_widget.geometry())
        self.locate_pane = LocatePane(parent=self.locate_layout)

        self.follow_layout = QHBoxLayout()
        self.follow_layout.setGeometry(self.main_widget.geometry())
        self.follow_pane = FollowPane(parent=self.follow_layout)

        # --------------- LOGIC RELATED --------------------
        # state handler
        self.state = ['idle']  # resting when initialized

        # Keys allowed in the locate pane
        # 1. for pause/restart
        self.function_keys = [Qt.Key_Enter,
                              Qt.Key_Return,
                              Qt.Key_Space]
        # 2. arrow keys
        self.arrow_keys = {Qt.Key_Left,
                           Qt.Key_Up,
                           Qt.Key_Right,
                           Qt.Key_Down}

        # register for event filter
        qApp.installEventFilter(self)

        self.repeat_times = repeat_times
        self.remaining_repeat_times = self.repeat_times

        # --------------- cursor control --------------------
        self.cursor_home_pos = [int(x / 2) for x in screen_size]

        """
        # A list of points the cursor went through
        """
        self.trace = []

    def show(self) -> None:
        self.showFullScreen()

    def open_locate_pane(self):
        self.setWindowTitle("Locate")
        self.main_widget.setLayout(self.locate_layout)

    def close_locate_pane(self):
        self.locate_layout.removeWidget(self.locate_pane)

    def open_follow_pane(self):
        self.setWindowTitle("Follow")
        self.main_widget.setLayout(self.follow_layout)

    def close_follow_pane(self):
        self.follow_layout.removeWidget(self.follow_pane)

    def home_cursor(self):
        """Puts the cursor to the center of the canvas"""
        pyautogui.moveTo(*self.cursor_home_pos)

    # def eventFilter(self, obj, event):
    #     """
    #     only allows key events and key presses
    #     :param obj:
    #     :param event:
    #     :return:
    #     """
    #     if event.type() == QtCore.QEvent.KeyPress:
    #         key = event.key()

    def keyPressEvent(self, key_event):
        print(key_event.key())
        key = key_event.key()
        if key in self.function_keys or key in self.arrow_keys:
            # process key inputs
            self.key_logic(key)
            # TODO change the status in the parent window
            # do not use running because the target might not be initialized when just started
            if 'ready' not in self.state:
                self.trace.append([time.time(), pag.position()])  # update the trace, add position and timestamp

                if 'locate' in self.state:
                    self.check_locate_target()  # check if target reached
                elif 'follow' in self.state:
                    self.check_follow_target()
        else:
            super().keyPressEvent(key_event)

    def key_logic(self, key):
        """
        if space_bar:
            exit()
            return exit

        if 'locate':
            if 'idle':
                if [ready] -> waiting for return/enter to be pressed
            elif 'running' + arrow keys pressed:
                move by arrow keys

        :return: if the window's interrupted
        """
        if key == Qt.Key_Space:
            self.reset()
            self.hide()
            return True

        # If in rest
        if 'idle' in self.state:
            # if to yet started
            if 'ready' in self.state:
                if key == QtCore.Qt.Key_Enter or key == QtCore.Qt.Key_Return:
                    if 'locate' in self.state:
                        print(self.state)
                        self.start_locate_task()
                    elif 'follow' in self.state:
                        print(self.state)
                        self.start_follow_task()

                    self.state_start()

        # if running
        if 'running' in self.state and key in self.arrow_keys:
            arrow_key_event(key)

        return False

    def check_locate_target(self):
        """
        #   if target under mouse:
        #       start a new round
        #       update remaining repeat times
        #   if the task is completed:
        #       call finish_up()
        """
        if self.locate_pane.activated_target:
            if self.locate_pane.targets[self.locate_pane.activated_target].underMouse():
                self.remaining_repeat_times -= 1
                self.locate_pane.random_switch()

        # if self.is_completed():  # TODO @Nene I commented this part, because is_completed() is not defined
        #     self.finish_up()

    def check_follow_target(self):
        """
        #   if target under mouse:
        #       move the target to a new place
        #       update remaining repeat times
        #   if the task is completed:
        #       call finish_up()
        """
        if self.follow_pane.target.underMouse():
            self.remaining_repeat_times -= 1
            self.follow_pane.move_target(*pyautogui.position())

        # if completed
        if self.remaining_repeat_times == 0:
            self.finish_up()

    def finish_up(self):
        print("Task finished")
        print(self.trace)
        self.reset()
        self.hide()
        #  TODO change the running state in the parent window

    def reset(self):
        self.trace = []  # reset trace
        self.state = ['idle']
        self.remaining_repeat_times = self.repeat_times
        if self.locate_pane.activated_target:
            self.locate_pane.targets[self.locate_pane.activated_target].turn_off()

        self.follow_pane.target.turn_off()
        self.follow_pane.target.move(self.cursor_home_pos[0], self.cursor_home_pos[1])

    def start_locate_task(self):
        # put the cursor to the origin of the window
        # activate the locate pane
        self.main_widget.setLayout(self.locate_layout)
        self.home_cursor()
        self.locate_pane.activate()

    def start_follow_task(self):
        # put the cursor to the origin of the window
        # activate the locate pane
        self.main_widget.setLayout(self.follow_layout)
        self.home_cursor()
        self.follow_pane.target.move(self.cursor_home_pos[0], self.cursor_home_pos[1])
        self.follow_pane.activate()

    def state_start(self):
        print('started')
        self.state.remove('idle')
        self.state.remove('ready')
        self.state.append('running')

    """

    def state_pause(self):
        print('pause')
        self.state.remove('running')
        self.state.append('idle')

    def state_resume(self):
        print('resumed')
        self.state.remove('idle')
        self.state.append('running')
    
    """
