import os
from PyQt5.QtWidgets import QGraphicsScene, \
    QGraphicsView
from PyQt5.QtGui import QBrush, QPen, QTransform
from mGesf.main_page_tabs.gesture_tab import help_btn_action, generate_char_set
from mGesf.main_page_tabs.gesture_tab.indexPen.key_detection import ReturnKeyDetectionWidget

from utils.GUI_main_window import *
from utils.GUI_operation_tab import *
from mGesf.sound import *
import config
import pyqtgraph as pg


# TODO ISSUE: there's a perceptible glitch in the runtime graphs when the metronome refreshes, will this go away if
#  the frame rate is lower when the sensors are connected (at 30FPS)?

class IdpRecording(QWidget):
    def __init__(self):
        super().__init__()
        # -------------------- First class --------------------
        self.main_page = QHBoxLayout(self)
        self.setLayout(self.main_page)

        # -------------------- Second class --------------------
        # 1. input block (vertical)
        # 2. instruction block (horizontal)
        self.input_block = init_container(parent=self.main_page,
                                          vertical=True,
                                          style=None)

        self.instruction_block = init_container(parent=self.main_page,
                                                vertical=False,
                                                style="background-color: white;",
                                                size=config.instruction_block_size)
        # -------------------- third class --------------------
        #   1. Input block
        #       1-1. Interval last
        #       1-2. Repeats
        #       1-3. Classes
        #       1-4. Subject name
        #       1-5. Training data directory
        #       1-6. Buttons + help

        self.interval_last_block, self.interval_slider_view = init_slider_bar_box(self.input_block,
                                                                                  label=config.operation_interval_label,
                                                                                  interval=config.recording_interval)

        self.repeats_block, self.repeat_slider_view = init_slider_bar_box(self.input_block,
                                                                          label=config.operation_repeats_label,
                                                                          interval=config.repeat_times)

        self.classes_block, self.classes_textbox = init_inputBox(self.input_block,
                                                                 label=config.operation_classes_label,
                                                                 label_bold=False,
                                                                 default_input=config.indexPen_classes_default)

        self.subject_name_block, self.subject_names_textbox = init_inputBox(self.input_block,
                                                                            label=config.operation_subject_name_label,
                                                                            label_bold=False,
                                                                            default_input=
                                                                            config.indexPen_subjectName_default)

        self.training_dir_block, self.training_dir_textbox = init_inputBox(self.input_block,
                                                                           label=config.trainingDataPath_label,
                                                                           label_bold=False,
                                                                           default_input=
                                                                           config.indexPen_trainingDataDir_default)

        # -------------------- fourth class --------------------
        #   1-6. Buttons + help (horizontally distributed)
        #       1-6-1. Buttons
        #       1-6-2. Help (message box)

        self.buttons_block = init_container(self.input_block, vertical=False)
        self.help_block = init_container(self.input_block)

        # -------------------- fifth class --------------------
        #       1-6-1. Buttons
        #           1-6-1-1. Interrupt
        #           1-6-1-2. Start/end test
        #           1-6-1-3. Start Recording

        self.interrupt_btn = init_button(parent=self.buttons_block,
                                         label=config.interrupt_btn_label,
                                         function=self.interrupt_btn_action)

        self.test_btn = init_button(parent=self.buttons_block,
                                    label=config.test_btn_start_label,
                                    function=self.test_btn_action)

        self.recording_btn = init_button(parent=self.buttons_block,
                                         label=config.record_btn_start_label,
                                         function=self.recording_btn_action)

        self.help_btn = init_button(parent=self.buttons_block,
                                    label=config.help_btn_label,
                                    function=help_btn_action)

        # -------------------- third class --------------------
        #   1. Instruction block (horizontal)
        #       1-1. circles block (vertical)
        #       1-2. text block (vertical)

        self.counter_block = init_container(parent=self.instruction_block, vertical=True,
                                            size=config.counter_block_size)
        self.ist_text_block = init_container(parent=self.instruction_block, vertical=True,
                                             size=config.ist_text_block_size)
        # -------------------- fourth class --------------------
        #       1-1. circles block (vertical)
        #           1-1-1. circles_view
        #               1-1-1-1. circles_scene
        #                   1-1-1-1. 4 circles drawn to the scene

        self.metronome_scene = QGraphicsScene()
        self.metronome_view = QGraphicsView(self.metronome_scene)
        # positions of the four circles
        # stored for redraw
        self.x1, self.x2, self.x3, self.x4, self.y1, self.y2, self.y3, self.y4, self.circle_scene_width, \
        self.circle_scene_height = self.setup_canvas()
        # show the circles
        self.paint(first_circle_colored=False)

        # -------------------- fourth class --------------------
        #       1-2. text block (vertical)
        #           ------- preparation -------
        #           1-1-1. "You will be writing:  "
        #                  char_set ::= A sequence of text the user will write
        #                  "Press Enter To Continue"
        #
        #           ------- forecast ------
        #           1-1-1. Label1: Forecast
        #
        #           ------- record ------
        #           1-1-1. Label1: "Write"
        #                  character to write
        #                  "...next" + next character to write

        # will be initialized when the user presses enter/return after preparation
        self.countdown_block, self.countdown_label = None, None
        # will be initialized when the forecast animation is over
        self.lb_char_to_write, self.lb_char_next = None, None

        self.show()

        # ============================= others ==========================

        # get input values
        self.interval = self.interval_slider_view.slider.value()
        self.repeat_times = self.repeat_slider_view.slider.value()
        # stored in a list
        self.classes = self.get_classes()
        self.subject_name = self.get_subject_name()
        self.training_dir = self.get_training_data_dir()
        self.char_set = generate_char_set(self.classes, self.repeat_times)

        # =========================== timers =============================
        self.timer = QtCore.QTimer()
        self.timer.setInterval(self.interval * 1000)
        self.timer.timeout.connect(self.ticks)
        self.timer.start()

        # ======================= indicators, counters ==========================
        self.state = ['idle']  # see the docstring of self.update_state for details

        # tracking if the user pressed the return key to start recording
        self.is_dir_valid = False
        self.cur_countdown, self.tempo_counter = 0, 0

        self.reset_instruction()

    def update_state(self, action):
        """
        update the current state based on action
        The working states, as oppose to 'idle' include that of 'pending', 'testing', 'countingDown', 'writing'
        @param action: str: issued with the following functions with the corresponding value:
            * self.keyPressEvent(): 'enter_pressed'
            * self.countdown_tick(): 'countdown_over'
            * self.test_btn_action(): 'start_test'
            * self.update_state: 'countdown_over'  @@for restarting writing when in test mode
        @note: you will see that there's a slight pause at the start of writing and the instruction says writing '...',
                this is an expected behavior as we are not updating the states in a clock, but rather posting to the
                state changes in function class. It's not a bug, it's a feature!
        """
        if action == 'test_pressed':
            if 'idle' in self.state:  # start the test mode
                self.idle_to_pending()
                self.state = ['testing', 'pending']
            else:  # back to idle
                self.update_state('interrupt')  # this is equivalent to issuing an interrupt action
        # break pending state and start count down
        elif action == 'enter_pressed':
            if 'pending' in self.state:
                self.state.remove('pending')
                self.state.append('countingDown')
                self.pending_to_countdown()
        elif action == 'countdown_over':
            if 'countingDown' in self.state:  # countingDown may not be in the states if looping in test mode
                self.state.remove('countingDown')
            self.state.append('writing')
            self.countdown_to_writing()
        elif action == 'writing_over':
            self.state.remove('writing')
            if 'testing' in self.state:  # restart writing if in test mode
                self.update_state('countdown_over')
            elif '':
                pass  # TODO implement writing over when in recording mode
            else:
                raise Exception('Unknown State change')
        elif action == 'interrupt':
            self.working_to_idle()  # working includes that
            self.state = ['idle']
        else:
            raise Exception('Unknown State change')
        self.resolve_state()

    def resolve_state(self):
        if 'testing' in self.state:
            self.test_btn.setText(config.test_btn_end_label)
            self.recording_btn.setDisabled(True)
        else:
            self.test_btn.setText(config.test_btn_start_label)
            self.recording_btn.setDisabled(False)

    def idle_to_pending(self):
        self.get_experiment_config()
        init_preparation_block(parent=self.ist_text_block, text=self.char_set)

    def working_to_idle(self):
        self.reset_instruction()

    def pending_to_countdown(self):
        # clear the preparation text block
        self.reset_instruction()
        # create the forecast block
        self.countdown_block, self.countdown_label = init_countdown_block(self.ist_text_block,
                                                                          label="   ",
                                                                          font=36,
                                                                          bold=True)

    def countdown_to_writing(self):
        clear_layout(self.ist_text_block)
        self.reset_instruction()
        self.lb_char_to_write, self.lb_char_next = init_instruction_text_block(self.ist_text_block)

    def keyPressEvent(self, key_event):
        print(key_event)
        if is_enter_key_event(key_event):
            self.update_state('enter_pressed')

    @pg.QtCore.pyqtSlot()
    def ticks(self):
        """
        check the current state
        ticks every 'refresh' milliseconds
        """

        if 'pending' in self.state:
            pass
        elif 'countingDown' in self.state:
            self.countdown_tick()
        elif 'writing' in self.state:
            self.metronome_tick()

    @pg.QtCore.pyqtSlot()
    def countdown_tick(self):
        # after the text block shows all countdown texts, stop updating, do nothing and return
        if self.cur_countdown == len(config.countdown_animation_text):
            self.cur_countdown += 1
            return
        # wait for a given interval and start the input timer
        elif self.cur_countdown > len(config.countdown_animation_text):  # end of counting down
            self.update_state('countdown_over')
        else:
            self.countdown_label.setText(config.countdown_animation_text[self.cur_countdown])
            self.cur_countdown += 1

    def metronome_tick(self):
        # tempo: dah, dih, dih,dih
        self.repaint(circle=self.tempo_counter % 4 + 1)
        if not self.tempo_counter % 4:
            dah()
            char_count = int(self.tempo_counter / 4)
            if char_count < len(self.char_set):
                # draw a new one
                cur_char = self.char_set[char_count]
                next_char = 'no Next' if (char_count + 1) == len(self.char_set) else self.char_set[char_count + 1]
                self.lb_char_to_write.setText(cur_char)
                self.lb_char_next.setText(config.instruction_next_text + next_char)
            # finish a recording loop
            else:
                self.update_state('writing_over')
                # must return here to avoid further incrementing the tempo counter, it is reset within update_state()
                return
        else:
            dih()

        self.tempo_counter += 1

    def setup_canvas(self):

        self.counter_block.addWidget(self.metronome_view)
        self.metronome_view.resize(config.unit_size, config.WINDOW_HEIGHT / 3)
        position = self.metronome_view.pos()

        self.metronome_scene.setSceneRect(position.x(), position.y(), self.metronome_view.width(),
                                          self.metronome_view.height())

        # size of the scene
        width = self.metronome_scene.width()
        height = self.metronome_scene.height()

        # positions of circles
        x1, y1 = position.x() + width / 3, position.y()
        x2, y2 = position.x() + width / 3, position.y() + 3 * (height / 10)
        x3, y3 = position.x() + width / 3, position.y() + 6 * (height / 10)
        x4, y4 = position.x() + width / 3, position.y() + 9 * (height / 10)

        return x1, x2, x3, x4, y1, y2, y3, y4, width, height

    def repaint(self, circle=1):

        x = self.x1
        y = self.y1

        if circle == 1:
            # remove all current circles
            for item in self.metronome_scene.items():
                self.metronome_scene.removeItem(item)
            # paint them light gray
            self.paint()

        else:
            if circle == 2:
                x = self.x2
                y = self.y2
            elif circle == 3:
                x = self.x3
                y = self.y3
            elif circle == 4:
                x = self.x4
                y = self.y4

        # locate the circle
        circle = self.metronome_scene.itemAt(x, y, QTransform())
        # remove the original circle
        if circle:
            self.metronome_scene.removeItem(circle)

        # repaint a blue one
        pen = QPen(Qt.black, 5, Qt.SolidLine)
        brush = QBrush(Qt.blue, Qt.SolidPattern)
        self.metronome_scene.addEllipse(x, y,
                                        self.circle_scene_width / 3, self.circle_scene_width / 3,
                                        pen, brush)

    def paint(self, first_circle_colored=False):

        pen = QPen(Qt.black, 5, Qt.SolidLine)
        brush = QBrush(Qt.lightGray, Qt.SolidPattern)

        self.metronome_scene.addEllipse(self.x2, self.y2,
                                        self.circle_scene_width / 3, self.circle_scene_width / 3,
                                        pen, brush)
        self.metronome_scene.addEllipse(self.x3, self.y3,
                                        self.circle_scene_width / 3, self.circle_scene_width / 3,
                                        pen, brush)
        self.metronome_scene.addEllipse(self.x4, self.y4,
                                        self.circle_scene_width / 3, self.circle_scene_width / 3,
                                        pen, brush)

        if first_circle_colored:
            brush = QBrush(Qt.blue, Qt.SolidPattern)

        self.metronome_scene.addEllipse(self.x1, self.y1,
                                        self.circle_scene_width / 3, self.circle_scene_width / 3,
                                        pen, brush)

    def get_experiment_config(self):
        self.interval = self.interval_slider_view.slider.value()
        self.repeat_times = self.repeat_slider_view.slider.value()
        self.classes = self.get_classes()
        self.subject_name = self.get_subject_name()
        self.training_dir = self.get_training_data_dir()

    def get_training_data_dir(self):
        _user_input = self.training_dir_textbox.text()
        if not _user_input:
            _user_input = config.indexPen_trainingDataDir_default

        return _user_input

    def get_subject_name(self):
        _user_input = self.subject_names_textbox.text()
        if not _user_input:
            _user_input = config.indexPen_subjectName_default
        return _user_input

    def get_classes(self):
        _user_input = self.classes_textbox.text()
        if not _user_input:
            _user_input = config.indexPen_classes_default

        # change to a list
        classes = _user_input.split(" ")
        return classes

    def interrupt_btn_action(self):
        self.update_state('interrupt')

    def test_btn_action(self):
        self.update_state('test_pressed')

    def recording_btn_action(self):
        # TODO implement this action
        pass
        # if self.is_recording:
        #     self.reset()
        #
        # # if not recording yet
        # elif not self.is_recording:
        #
        #     self.get_experiment_config()
        #     # try starting recording
        #     # check the data path first
        #     self.is_dir_valid = self.check_dir_valid()
        #
        #     if self.is_dir_valid:
        #         # if valid data path, show preparation page
        #         # start recording
        #         self.is_recording = True
        #         self.prepare()
        #         self.recording_btn.setText(config.record_btn_end_label)
        #         self.countdown_timer.start()
        #
        #         msg = QMessageBox()
        #         msg.setIcon(QMessageBox.Information)
        #         msg.setText("Recording")
        #         msg.exec()

    def check_dir_valid(self):
        print(self.training_dir)
        if os.path.exists(self.training_dir):
            return True
        else:
            msg = QMessageBox()
            msg.setIcon(QMessageBox.Information)
            msg.setText(config.datapath_invalid_message)
            msg.exec()
            return False

    def reset_instruction(self):
        clear_layout(self.ist_text_block)
        for item in self.metronome_scene.items():
            self.metronome_scene.removeItem(item)
        self.paint()
        self.lb_char_next, self.lb_char_to_write = None, None
        self.cur_countdown, self.tempo_counter = 0, 0


def is_enter_key_event(key_event):
    return key_event.key() == QtCore.Qt.Key_Return or key_event.key() == QtCore.Qt.Key_Enter
