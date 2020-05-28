from utils.GUI_main_window import *
from utils.GUI_operation_tab import *
from mGesf.main_page_tabs.gesture_tab.thuMouse.Locate_Pane import Locate_Pane
import config
import os
import pyautogui as pag

# how many pixels does the cursor move in one step
step = 3


class Recording(QWidget):
    def __init__(self):
        super().__init__()

        # for checking only one box
        self._toggle = None

        # the timer
        self.timer = QtCore.QTimer()
        self.timer.setInterval(1000)
        self.timer.timeout.connect(self.ticks)

        # ==================  layouts  ==========================
        # -------------------- First class --------------------
        self.main_page = QHBoxLayout(self)
        self.setLayout(self.main_page)

        # -------------------- Second class --------------------
        # 1. input block (vertical)
        # 2. recording block
        self.input_block = init_container(parent=self.main_page, vertical=True)
        self.recording_block = init_container(parent=self.main_page, vertical=False)

        # -------------------- third class --------------------
        #   1. Input block
        #       1-1. task selection box (horizontal)
        #       1-2. subject name textbox
        #       1-3. training dir textbox
        #       1-4. buttons
        self.taskSelection_block = init_container(parent=self.input_block,
                                                  label="Task Selection",
                                                  vertical=True)

        self.subjectName_block, self.subjectName_textbox = init_inputBox(parent=self.input_block,
                                                                         label="Subject Name",
                                                                         default_input=config.thuMouse_subjectName_default)

        self.trainingDataDir_block, self.trainingDataDir_textbox = init_inputBox(parent=self.input_block,
                                                                                 label="Training Data Directory",
                                                                                 default_input=config.thuMouse_TrainingDataDir_default)

        self.buttons_block = init_container(parent=self.input_block,
                                            vertical=False)

        # -------------------- fourth class --------------------
        #   1-1. task selection box (vertical)
        #       1-1-1. follow checkbox
        #       1-1-2. locate checkbox

        self.follow_checkbox = init_checkBox(parent=self.taskSelection_block,
                                             label="follow",
                                             function=self.follow_checkBox_action)

        self.locate_checkbox = init_checkBox(parent=self.taskSelection_block,
                                             label="locate",
                                             function=self.locate_checkBox_action)

        # -------------------- fourth class --------------------
        #       1-4. Buttons
        #           1-4-1. Interrupt
        #           1-4-2. Start/end test
        #           1-4-3. Start Recording

        self.interrupt_btn = init_button(parent=self.buttons_block,
                                         label=config.interrupt_btn_label,
                                         function=self.interrupt_btn_action)

        self.test_btn = init_button(parent=self.buttons_block,
                                    label=config.test_btn_start_label,
                                    function=self.test_btn_action)

        self.recording_btn = init_button(parent=self.buttons_block,
                                         label=config.record_btn_start_label,
                                         function=self.recording_btn_action)

        # -------------------- third class --------------------
        #   1. recording block
        #       1-1. Follow canvas
        #       -------------------
        #       1-1. locate canvas (vertical)
        #           1-1-1. circle1 + circle3 (vertical)
        #           1-1-2. circle2 + circle4 (vertical)

        # locate canvas
        # initialized when the self.state = ['locating', 'testing'] or ['locating', 'recording']
        self.locate_canvas = None
        self.locate_canvas = Locate_Pane(parent=self.recording_block,
                                         rows=3,
                                         columns=3)
        self.subject_name = self.get_subject_name()
        self.training_dir = self.get_training_data_dir()
        self.state = ['idle']  # see the docstring of self.update_state for details

        # position of the cursor
        self.cursor_x = None
        self.cursor_y = None

    @pg.QtCore.pyqtSlot()
    def ticks(self):
        """
        ticks every 'refresh' milliseconds
        """
        print("tick")

    def activate_follow_panel(self):
        print('follow')

    def activate_locate_panel(self):
        print('locate')

    def start_test_locate(self):
        self.put_cursor_to_center()

    def start_test_follow(self):
        print("testing follow")

    def update_state(self, action):
        """
        update the current state based on action
        The working states, as oppose to 'idle' include that of 'pending', 'testing', 'countingDown', 'writing'
        @param action: str: issued with the following functions with the corresponding value:
            * self.follow_checkBox_action(): 'follow'
            * self.locate_checkBox_action(): 'locate'
            * self.recording_btn_action(): 'start_recording'
            * self.test_btn_action(): 'start_test'
            * self.update_state: 'countdown_over'
        """
        if action == 'follow':
            # if locate is chosen, remove it
            if 'locate' in self.state:
                self.state.remove('locate')
            self.state.append(action)

        elif action == 'not_follow':
            if 'follow' in self.state:
                self.state.remove('follow')

        elif action == 'locate':
            if 'follow' in self.state:
                self.state.remove('follow')
            self.state.append(action)

        elif action == 'not_locate':
            if 'locate' in self.state:
                self.state.remove('locate')

        elif action == 'test_pressed':
            if 'idle' in self.state:  # start the test mode
                if 'locate' in self.state:
                    self.idle_to_testing()
                    self.state.remove('idle')
                    self.state.append('testing')

                    self.start_test_locate()

                elif 'follow' in self.state:
                    self.idle_to_testing()
                    self.state.remove('idle')
                    self.state.append('testing')

                    self.start_test_follow()

                else:
                    print("Select a mode")

            elif action == "up_pressed":
                self._cursor_up()
            elif action == "down_pressed":
                self._cursor_down()
            elif action == "left_pressed":
                self._cursor_left()
            elif action == "right_pressed":
                self._cursor_right()

            else:  # back to idle
                self.update_state('interrupt_pressed')  # this is equivalent to issuing an interrupt action

        elif action == 'record_pressed':
            if 'idle' in self.state:  # start the test mode
                if 'locate' in self.state or 'follow' in self.state:
                    # start testing
                    self.idle_to_recording()
                    self.state.remove('idle')
                    self.state.append('recording')
                else:
                    print("Select a mode")
            else:  # back to idle
                self.update_state('interrupt_pressed')  # this is equivalent to issuing an interrupt action

        elif action == 'interrupt_pressed':
            if 'testing' in self.state:
                self.state.remove('testing')
            elif 'recording' in self.state:
                self.state.remove('recording')
            # pause working anyway
            self.pause_working()

        else:
            raise Exception('Unknown State change')
        self.resolve_state()

    def follow_checkBox_action(self):
        if self.follow_checkbox.isChecked():
            self.update_state("follow")
            self._toggle = True
            self.locate_checkbox.setChecked(not self._toggle)
            print(self.state)

        else:
            self.update_state('not_follow')
            self._toggle = not self._toggle
            print(self.state)

        return

    def locate_checkBox_action(self):
        if self.locate_checkbox.isChecked():
            self.update_state("locate")
            self._toggle = True
            self.follow_checkbox.setChecked(not self._toggle)
            print(self.state)

        else:
            self.update_state('not_locate')
            self._toggle = not self._toggle
            print(self.state)

        return

    def get_training_data_dir(self):
        _user_input = self.trainingDataDir_textbox.text()
        if not _user_input:
            _user_input = config.thuMouse_TrainingDataDir_default

        return _user_input

    def get_subject_name(self):
        _user_input = self.subjectName_textbox.text()
        if not _user_input:
            _user_input = config.thuMouse_subjectName_default
        return _user_input

    def check_dir_valid(self):
        if os.path.exists(self.training_dir):
            return True
        else:
            msg = QMessageBox()
            msg.setIcon(QMessageBox.Information)
            msg.setText(config.datapath_invalid_message)
            msg.exec()
            return False

    def test_btn_action(self):
        self.update_state('test_pressed')
        print(self.state)

        return

    def interrupt_btn_action(self):
        self.update_state('interrupt_pressed')
        print(self.state)

        return

    def recording_btn_action(self):
        self.update_state('record_pressed')
        print(self.state)

        return

    def resolve_state(self):
        if 'testing' in self.state:
            self.test_btn.setText(config.test_btn_end_label)
            self.recording_btn.setDisabled(True)
        else:
            self.test_btn.setText(config.test_btn_start_label)
            self.recording_btn.setDisabled(False)

        if 'recording' in self.state:
            self.recording_btn.setText(config.record_btn_end_label)
            self.test_btn.setDisabled(True)
        else:
            self.recording_btn.setText(config.record_btn_start_label)
            self.test_btn.setDisabled(False)

    def pause_working(self):
        self.state.append('idle')
        self.follow_checkbox.setDisabled(False)
        self.locate_checkbox.setDisabled(False)
        print("paused")

    def idle_to_recording(self):
        self.follow_checkbox.setDisabled(True)
        self.locate_checkbox.setDisabled(True)
        print('recording')

    def idle_to_testing(self):
        self.follow_checkbox.setDisabled(True)
        self.locate_checkbox.setDisabled(True)
        print('testing')

    def put_cursor_to_center(self):
        """Puts the cursor to the center of the canvas"""
        if 'locate' in self.state:
            self.cursor_x, self.cursor_y = self.locate_canvas.x(), self.locate_canvas.y()
            print(str(self.cursor_x) + " " + str(self.cursor_y))
            pag.move(self.cursor_x, self.cursor_y)

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
        print(key_event)
        if key_event.key() == QtCore.Qt.Key_Up:
            self.update_state('up_pressed')

        if key_event.key() == QtCore.Qt.Key_Down:
            self.update_state('down_pressed')

        if key_event.key() == QtCore.Qt.Key_Left:
            self.update_state('left_pressed')

        if key_event.key() == QtCore.Qt.Key_Right:
            self.update_state('right_pressed')

