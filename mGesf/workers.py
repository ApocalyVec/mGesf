import time
import numpy as np

from PyQt5.QtCore import pyqtSignal, QObject
import pyqtgraph as pg

import config
from utils.simulation import sim_heatmap, sim_detected_points, sim_imp

import mGesf.exceptions as exceptions

import numpy as np


class MmwWorker(QObject):
    """
    mmw data package (dict):
        'range_doppler': ndarray
        'range_azi': ndarray
        'pts': ndarray
        'range_amplitude' ndarray
    """
    # for passing data to the gesture tab
    signal_mmw_gesture_tab = pyqtSignal(dict)
    # for passing data to the radar tab
    signal_mmw_radar_tab = pyqtSignal(dict)
    # for passing data to the control tab
    signal_mmw_control_tab = pyqtSignal(dict)

    tick_signal = pyqtSignal()
    timing_list = []  # TODO refactor timing calculation

    def __init__(self, mmw_interface=None, *args, **kwargs):
        super(MmwWorker, self).__init__()
        self.tick_signal.connect(self.mmw_process_on_tick)
        if not mmw_interface:
            print('None type mmw_interface, starting in simulation mode')

        self._mmw_interface = mmw_interface
        self._is_running = False

    @pg.QtCore.pyqtSlot()
    def mmw_process_on_tick(self):
        if self._is_running:
            if self._mmw_interface:
                try:
                    start = time.time()
                    pts_array, range_amplitude, rd_heatmap, azi_heatmap = self._mmw_interface.process_frame()
                except exceptions.DataPortNotOpenError:  # happens when the emitted signal accumulates
                    return
                if range_amplitude is None:  # replace with simulated data if not enabled
                    range_amplitude = sim_imp()
                if rd_heatmap is None:
                    rd_heatmap = sim_heatmap(config.rd_shape)
                if azi_heatmap is None:
                    azi_heatmap = sim_heatmap(config.ra_shape)
                self.timing_list.append(time.time() - start)  # TODO refactor timing calculation

            else:  # this is in simulation mode
                pts_array = sim_detected_points()
                range_amplitude = sim_imp()
                rd_heatmap = sim_heatmap(config.rd_shape)
                azi_heatmap = sim_heatmap(config.ra_shape)

            # notify the mmw data for the radar tab
            data_dict = {'range_doppler': rd_heatmap,
                         'range_azi': azi_heatmap,
                         'pts': pts_array,
                         'range_amplitude': range_amplitude}
            self.signal_mmw_radar_tab.emit(data_dict)
            self.signal_mmw_control_tab.emit(data_dict)  # notify the mmw data for the control tab
            self.signal_mmw_gesture_tab.emit(data_dict)  # notify the mmw data for the gesture tab

    def start_mmw(self):
        if self._mmw_interface:  # if the sensor interface is established
            try:
                self._mmw_interface.start_sensor()
            except exceptions.PortsNotSetUpError:
                print('Radar COM ports are not set up, connect to the sensor prior to start the sensor')
        else:
            print('Start Simulating mmW data')
            # raise exceptions.InterfaceNotExistError
        self._is_running = True

    def stop_mmw(self):
        self._is_running = False
        time.sleep(0.1)  # wait 100ms for the previous frames to finish process
        if self._mmw_interface:
            self._mmw_interface.stop_sensor()
            print('frame rate is ' + str(1 / np.mean(self.timing_list)))  # TODO refactor timing calculation
        else:
            print('Stop Simulating mmW data')
            print('frame rate calculation is not enabled in simulation mode')

    def is_mmw_running(self):
        return self._is_running

    def is_connected(self):
        if self._mmw_interface:
            return self._mmw_interface.is_connected()
        else:
            print('No Radar Interface Connected, ignored.')
            # raise exceptions.InterfaceNotExistError

    def connect_mmw(self, uport_name, dport_name):
        """
        check if _mmw_interface exists before connecting.
        """
        if self._mmw_interface:
            self._mmw_interface.connect(uport_name, dport_name)
        else:
            print('No Radar Interface Connected, ignored.')
            # raise exceptions.InterfaceNotExistError

    def disconnect_mmw(self):
        """
        check if _mmw_interface exists before disconnecting.
        """
        self.stop_mmw()
        if self._mmw_interface:
            self._mmw_interface.close_connection()
        else:
            print('No Radar Interface Connected, ignored.')
            # raise exceptions.InterfaceNotExistError

    def send_config(self, config_path):
        """
        check if _mmw_interface exists before sending the config path.
        """
        if self._mmw_interface:
            self._mmw_interface.send_config(config_path)
            self.start_mmw()
        else:
            print('No Radar Interface Connected, ignored.')
            # raise exceptions.InterfaceNotExistError


class IdpDetectionWorker(QObject):
    """
    detection result package (dict):
        'pred': ndarray, decoded argmax of the output
        'output': ndarray, output vector of the prediction model, shape = n * number of classes
    """
    tick_signal = pyqtSignal(dict)
    signal_detection = pyqtSignal(dict)

    def __init__(self, *args, **kwargs):
        super(IdpDetectionWorker, self).__init__()
        self.tick_signal.connect(self.detect_on_tick)
        self.encoder = None
        self.model = None

    def detect_on_tick(self, samples):
        output = self.model.predict([np.array(samples['rd']),
                            np.array(samples['ra'])])
        pred = self.encoder.inverse_transform(output)
        self.signal_detection.emit({'pred': pred,
                                    'output': output})  # notify the mmw data for the gesture tab

    def setup(self, encoder, model):
        self.encoder = encoder
        self.model = model

