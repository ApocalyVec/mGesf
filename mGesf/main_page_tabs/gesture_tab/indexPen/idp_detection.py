from PyQt5.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QSlider, QLabel

from mGesf.main_page_tabs.gesture_tab import help_btn_action
from mGesf.main_page_tabs.gesture_tab.indexPen.prob_view import ProbViewWindow
from mGesf.workers import IdpDetectionWorker
from utils.GUI_main_window import *
from utils.GUI_operation_tab import *
import config
import pyqtgraph as pg

import tensorflow as tf

class IdpDetection(QWidget):
    def __init__(self, mmw_signal):  # TODO add a slider for setting the detection interval
        super().__init__()

        # -------------------- First class --------------------
        self.main_page = QHBoxLayout(self)
        self.setLayout(self.main_page)

        # -------------------- Second class --------------------
        # 1. input block
        # 2. detection output block
        self.input_block = init_container(parent=self.main_page, vertical=True)
        self.detection_output_block = init_container(parent=self.main_page, vertical=False,
                                                     size=config.dtc_text_block_size)

        """
        adding to detection output block
        """
        self.lb_char_written = init_detection_text_block(self.detection_output_block)

        # -------------------- third class --------------------
        #   1. Input block
        #       1-1. text boxes
        #       1-2. Buttons
        self.text_box_block = init_container(self.input_block, vertical=True)
        self.buttons_block = init_container(self.input_block, vertical=False)

        # -------------------- fourth class --------------------
        #   1-1. text boxes
        #       1-1-1. model path
        #       1-1-2. training data dir

        model_path_block, self.model_path_text = init_inputBox(self.text_box_block,
                                                               label=config.operation_model_path_label,
                                                               label_bold=False,
                                                               default_input=config.indexPen_modelPath_default)

        training_dir_block, self.training_dir_text = init_inputBox(self.text_box_block,
                                                                   label=config.trainingDataPath_label,
                                                                   label_bold=False,
                                                                   default_input=config.indexPen_trainingDataDir_default)

        # -------------------- fifth class --------------------
        #   1-6. Buttons
        #       1-6-1. load button
        #       1-6-2. start/stop detection button
        #       1-6-3. Help button

        self.load_btn = init_button(parent=self.buttons_block,
                                    label=config.detection_load_btn_label,
                                    function=self.load_btn_action)
        self.start_stop_detection_btn = init_button(parent=self.buttons_block,
                                                    label=config.detection_start_btn_label,
                                                    function=self.start_stop_btn_action)

        self.help_btn = init_button(parent=self.buttons_block,
                                    label=config.help_btn_label,
                                    function=help_btn_action)

        # init probability view window
        self.prob_view_btn = init_button(parent=self.buttons_block,
                                         label=config.prob_view_btn_label,
                                         function=self.prob_view_btn_action)
        self.prob_view_win = ProbViewWindow(self)

        # init signal processing
        mmw_signal.connect(self.detection_process_mmw)
        self.encoder = None  # idp classification categories
        self.timestep = None
        # create threading
        # create a QThread and start the thread that handles
        self.worker_thread = pg.QtCore.QThread(self)
        self.worker_thread.start()

        # worker
        self.dtc_worker = IdpDetectionWorker()
        self.dtc_worker.moveToThread(self.worker_thread)
        self.dtc_worker.signal_detection.connect(self.process_detection)

        # state variable
        self.prob_view_btn.setEnabled(False)
        self.is_detecting = False

        # temporal samples
        self.sample = {'rd': [],
                       'ra': [],
                       'foo': []}
        self.sample_acc = {'rd': [],  # accumulative samples,
                           'ra': [],
                           'foo': []}
        self.sample_count = 0
        self.interval_counter = 0
        self.dtc_interval = 11

        '''
        # allow dynamic allocation of GPU memory
        tf_config = tf.ConfigProto()
        tf_config.gpu_options.allow_growth = True
        session = tf.Session(config=tf_config)
        '''

    def load_btn_action(self):
        print('IndexPen Detection: Loading...')
        try:
            from keras.engine.saving import load_model
            self.encoder = retrieve_idp_encoder(self.training_dir_text.text())
            model = load_model(self.model_path_text.text())
            self.timestep = int(model.input[0].shape[1])
            self.dtc_worker.setup(self.encoder, model)
            self.prob_view_win.setup(self.encoder)
        except FileNotFoundError as e:
            print('IndexPen Detection: train data and/or model path not exist')
        print('IndexPen Detection: Load Complete')

    def start_stop_btn_action(self):
        if not self.is_detecting:
            self.load_btn_action()  # load the model before starting
            self.start_stop_detection_btn.setText(config.detection_end_btn_label)
            self.prob_view_btn.setEnabled(True)
            self.interval_counter = 0
            self.sample_count = 0
        else:
            self.start_stop_detection_btn.setText(config.detection_start_btn_label)
            self.prob_view_win.close()
            self.prob_view_btn.setEnabled(False)

        self.is_detecting = not self.is_detecting

    def prob_view_btn_action(self):
        self.prob_view_win.show()

    def detection_process_mmw(self, data_dict):
        if self.is_detecting:
            self.interval_counter += 1
            self.sample_count += 1
            self.sample['foo'] = circular_sampling(self.sample['foo'], point=self.sample_count, timestep=self.timestep)
            self.sample['rd'] = circular_sampling(sample=self.sample['rd'], point=data_dict['range_doppler'],
                                                  timestep=self.timestep)
            self.sample['ra'] = circular_sampling(sample=self.sample['ra'], point=data_dict['range_azi'],
                                                  timestep=self.timestep)

            if len(self.sample['rd']) >= self.timestep:  # if there are enough points to make a complete sample
                self.sample_acc['rd'].append(self.sample['rd'])
                self.sample_acc['ra'].append(self.sample['ra'])
                self.sample_acc['foo'].append(self.sample['foo'])
                if self.interval_counter >= self.dtc_interval:
                    self.interval_counter = 0
                    self.dtc_worker.tick_signal.emit(
                        self.sample_acc)  # signal the detection thread to make a prediction
                    self.clear_accumulative_samples()

    def clear_accumulative_samples(self):
        self.sample_acc = {'rd': [],  # accumulative samples,
                           'ra': [],
                           'foo': []}

    def process_detection(self, dtc_dict):
        print(str(dtc_dict['pred']))
        self.prob_view_win.signal_dtc_output.emit(dtc_dict['output'])
