from utils.GUI_main_window import *
from utils.GUI_operation_tab import *
from mGesf.main_page_tabs.gesture_tab.thuMouse.Interaction_window import Interaction_window
import config
import os


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
        #       1-2. repeat time textbox
        #       1-3. subject name textbox
        #       1-4. training dir textbox
        #       1-5. buttons
        self.taskSelection_block = init_container(parent=self.input_block,
                                                  label="Task Selection",
                                                  vertical=True)

        self.repeatTime_block, self.repeatTime_textbox = init_inputBox(parent=self.input_block,
                                                                       label="Repeat Times: ",
                                                                       default_input=config.thuMouse_repeatTimes_default)

        self.subjectName_block, self.subjectName_textbox = init_inputBox(parent=self.input_block,
                                                                         label="Subject Name: ",
                                                                         default_input=config.thuMouse_subjectName_default)

        self.trainingDataDir_block, self.trainingDataDir_textbox = init_inputBox(parent=self.input_block,
                                                                                 label="Training Data Directory: ",
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
        self.interaction_window = Interaction_window(self)
        self.subject_name = self.get_subject_name()
        self.training_dir = self.get_training_data_dir()
        self.state = ['idle']  # see the docstring of self.update_state for details

        # repeat times
        self.repeat_times = self.get_repeat_times()
        self.is_interactive_window_open = False

    def update_inputs(self):
        self.repeat_times = self.get_repeat_times()
        self.subject_name = self.get_subject_name()
        self.training_dir = self.get_training_data_dir()

    def restart(self):
        self.update_inputs()

        if 'testing' in self.state:
            self.state.remove('testing')
        if 'recording' in self.state:
            self.state.remove('recording')
        if 'idle' not in self.state:
            self.state.append('idle')

        self.enable_checkboxes()

        # close the interactive window and reset
        self.interaction_window.reset()
        self.interaction_window.hide()

    def get_repeat_times(self):
        _user_input = self.repeatTime_textbox.text()
        if not _user_input:
            _user_input = config.thuMouse_repeatTimes_default

        # make sure the input's int
        try:
            # TODO @ LEO THIS IS NOT WORKING. IT STILL QUITS IF THE INPUT'S NOT INT
            #  File "/Users/neneko/Documents/GitHub/mGesf/mGesf/main_page_tabs/gesture_tab/thuMouse/thm_recording.py",
            #  line 135, in get_repeat_times _user_input = int(_user_input) ValueError: invalid literal for int()
            #  with base 10: '4m'

            _user_input = int(_user_input)
        except AssertionError as ae:
            _user_input = config.thuMouse_repeatTimes_default
            print("Repeat times input should be an integer")

        return _user_input

    @pg.QtCore.pyqtSlot()
    def ticks(self):
        """
        ticks every 'refresh' milliseconds
        """
        print("tick")

    def activate_follow_pane(self):
        # set the interactive window to follow
        self.interaction_window.open_follow_pane()
        # update the state of the instruction window
        self.interaction_window.state.append('follow')
        self.interaction_window.state.append('ready')

        # move the target to a new place ratehr than the center
        self.interaction_window.show()

    def activate_locate_pane(self):
        # set the interactive window to locate
        self.interaction_window.open_locate_pane()
        # update the state of the instruction window
        self.interaction_window.state.append('locate')
        self.interaction_window.state.append('ready')

        self.interaction_window.show()

    def start_follow_test(self):
        print("testing follow")
        self.state.remove('idle')
        self.state.append('testing')

        self.disable_checkboxes()

        self.activate_follow_pane()

    def start_locate_test(self):
        self.state.remove('idle')
        self.state.append('testing')

        self.disable_checkboxes()

        self.activate_locate_pane()

    def start_follow_recording(self):
        print("recording follow")
        self.state.remove('idle')
        self.state.append('recording')

        self.disable_checkboxes()

        self.activate_follow_pane()

    def start_locate_recording(self):
        self.state.remove('idle')
        self.state.append('recording')

        self.disable_checkboxes()
        self.activate_locate_pane()

    def update_state(self, act):
        """
        update the current state based on action
        The working states, as oppose to 'idle' include that of 'pending', 'testing', 'countingDown', 'writing'
        @param act: str: issued with the following functions with the corresponding value:
            * self.follow_checkBox_action(): 'follow'
            * self.locate_checkBox_action(): 'locate'
            * self.recording_btn_action(): 'recording'
            * self.test_btn_action(): 'testing'
        """

        # check the checkbox logic
        if act in ['follow', 'not_follow', 'locate', 'not_locate']:
            self.check_locate_follow_logic(act)
        # test/record logic
        elif act == 'test_pressed':
            self.update_inputs()
            self.check_test_logic(act)
        elif act == 'record_pressed':
            self.update_inputs()
            self.check_record_logic(act)
        # stop
        elif act == 'interrupt_pressed':
            self.restart()
        else:
            raise Exception('Unknown State change')

        self.resolve_state()

    def check_record_logic(self, act):
        if 'idle' in self.state:  # start the test mode
            if 'locate' in self.state:
                self.start_locate_recording()
            elif 'follow' in self.state:
                self.start_follow_recording()
            else:
                print("Select a mode")

        else:  # stop
            self.update_state('interrupt_pressed')  # this is equivalent to issuing an interrupt action

    def check_test_logic(self, act):
        if 'idle' in self.state:  # start the test mode
            if 'locate' in self.state:
                self.start_locate_test()
            elif 'follow' in self.state:
                self.start_follow_test()
            else:
                print("Select a mode")

        else:  # stop
            self.update_state('interrupt_pressed')  # this is equivalent to issuing an interrupt action

    def check_locate_follow_logic(self, act):
        """
        can only choose one
        :return:
        """
        if act == 'follow':
            # if locate is chosen, remove it
            if 'locate' in self.state:
                self.state.remove('locate')
            self.state.append(act)

        elif act == 'not_follow':
            if 'follow' in self.state:
                self.state.remove('follow')

        elif act == 'locate':
            if 'follow' in self.state:
                self.state.remove('follow')
            self.state.append(act)

        elif act == 'not_locate':
            if 'locate' in self.state:
                self.state.remove('locate')

    def start_recording(self):
        # start testing
        self.follow_checkbox.setDisabled(True)
        self.locate_checkbox.setDisabled(True)
        print('recording')

        # update state
        self.state.remove('idle')
        self.state.append('recording')

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

        # update state
        if 'testing' in self.state:
            self.state.remove('testing')
        elif 'recording' in self.state:
            self.state.remove('recording')

        print(self.state)

        return

    def recording_btn_action(self):
        self.update_state('record_pressed')
        print("recording")
        print(self.state)

        return

    def disable_checkboxes(self):
        self.follow_checkbox.setDisabled(True)
        self.locate_checkbox.setDisabled(True)

    def enable_checkboxes(self):
        self.follow_checkbox.setDisabled(False)
        self.locate_checkbox.setDisabled(False)

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

    def set_interactive_win_closed(self):
        self.is_interactive_window_open = False

    def set_interactive_win_open(self):
        self.is_interactive_window_open = True
