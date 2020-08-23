import time
from collections import deque

import numpy as np

from PyQt5.QtCore import pyqtSignal, QObject
import pyqtgraph as pg

import config
from utils.decaWave_utils.DecaUWB_interface import UWBSensorInterface
from utils.simulation import sim_heatmap, sim_detected_points, sim_imp, sim_uwb, sim_leap, sim_xe4thru

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
                                    'output': output,
                                    'foo': np.array(samples['foo'])}, )  # notify the mmw data for the gesture tab

    def setup(self, encoder, model):
        self.encoder = encoder
        self.model = model


class UWBWorker(QObject):
    signal_data = pyqtSignal(dict)
    tick_signal = pyqtSignal()

    def __init__(self, uwb_interface_anchor: UWBSensorInterface = None, uwb_interface_tag: UWBSensorInterface = None,
                 *args, **kwargs):
        super(UWBWorker, self).__init__()
        self.tick_signal.connect(self.uwb_process_on_tick)
        if uwb_interface_anchor is None and uwb_interface_tag is None:
            print('None type uwb_interface, starting in simulation mode')

        self._uwb_interface_anchor = uwb_interface_anchor
        self._uwb_interface_tag = uwb_interface_tag

        self._is_running = False

    @pg.QtCore.pyqtSlot()
    def uwb_process_on_tick(self):
        if self._is_running:
            if self._uwb_interface_anchor.connected and self._uwb_interface_tag.connected:
                a_frame = self._uwb_interface_anchor.generate_frame()
                t_frame = self._uwb_interface_tag.generate_frame()

            else:  # this is in simulation mode
                a_frame = sim_uwb()
                t_frame = sim_uwb()

            # notify the uwb real imag data
            data_dict = {'a_frame': a_frame,
                         't_frame': t_frame}
            self.signal_data.emit(data_dict)  # notify the uwb data for the sensor tab

    def start_uwb(self):
        if not (
                self._uwb_interface_anchor is None and self._uwb_interface_tag is None):  # if the sensor interface is established
            print("start UWB sensor")
            # try:
            #     self._uwb_interface_anchor.connect_virtual_port('COM32')
            #     self._uwb_interface_tag.connect_virtual_port('COM30')
            # except exceptions.PortsNotSetUpError:
            #     print('UWB COM ports are not set up, connect to the sensor prior to start the sensor')
        else:
            print('Start Simulating UWB data')
        self._is_running = True


class LeapWorker(QObject):
    """
    leap data package (dict): leapmouse: [current vertical position, current horizontal position, current z position,
    new X position, new Y position]
    """
    # for passing data to the gesture tab
    signal_leap = pyqtSignal(dict)
    tick_signal = pyqtSignal()
    timing_list = []

    def __init__(self, leap_interface=None, *args, **kwargs):
        super(LeapWorker, self).__init__()
        self.tick_signal.connect(self.leap_process_on_tick)
        if not leap_interface:
            print('None type LeapInterface, starting in simulation mode')

        self._leap_interface = leap_interface
        self._is_running = False

    @pg.QtCore.pyqtSlot()
    def leap_process_on_tick(self):
        if self._is_running:
            if self._leap_interface:
                leapMouse_data, image = self._leap_interface.process_frame()
            else:
                leapMouse_data, image = sim_leap()
            data_dict = {'leapmouse': leapMouse_data,
                         'image': image}
            self.signal_leap.emit(data_dict)

    def start_leap(self):
        if self._leap_interface:  # if the sensor interface is established
            try:
                self._leap_interface.connect_sensor()
                self._leap_interface.start_sensor()
            except exceptions.LeapPortTimeoutError:
                print('LeapInterface requires LeapMouse running, if you do not have the LeapMotion hardware, '
                      'set LeapInterface to None')
        else:
            print('Start Simulating Leap data')
        self._is_running = True

    def stop_leap(self):
        self._is_running = False
        time.sleep(0.1)  # wait 100ms for the previous frames to finish process
        if self._leap_interface:
            self._leap_interface.stop_sensor()
            print('frame rate is ' + str(1 / np.mean(self.timing_list)))  # TODO refactor timing calculation
        else:
            print('Stop Simulating leap data')
            print('frame rate calculation is not enabled in simulation mode')


class Xe4ThruWorker(QObject):
    signal_data = pyqtSignal(dict)
    tick_signal = pyqtSignal()

    def __init__(self, xeThruX4Sensor_interface=None, *args, **kwargs):
        super(Xe4ThruWorker, self).__init__()
        self.tick_signal.connect(self.xe4thru_process_on_tick)
        self.xeThruX4Sensor_interface = xeThruX4Sensor_interface
        self._is_running = False

        self.center_frequency = 3  # default value
        if xeThruX4Sensor_interface is None:  # add ir spectrogram buffer if no interface is connected
            self.frame_buffer = deque(maxlen=200)

    @pg.QtCore.pyqtSlot()
    def xe4thru_process_on_tick(self):
        if self._is_running:
            if self.xeThruX4Sensor_interface:
                frame, baseband_frame, clutter_removal_frame, clutter_removal_baseband_frame = self.xeThruX4Sensor_interface.read_frame()
                ir_spectrogram = self.xeThruX4Sensor_interface.clutter_removal_baseband_history

                if frame is not None:
                    data_dict = {'frame': frame,
                                 'baseband_frame': baseband_frame,
                                 'clutter_removal_frame': clutter_removal_frame,
                                 'clutter_removal_baseband_frame': clutter_removal_baseband_frame,
                                 'ir_spectrogram': np.array(list(ir_spectrogram))}
                    # notify the uwb data for the sensor tab; only emit when a frame is available
                    self.signal_data.emit(data_dict)
            else:
                frame = sim_xe4thru()
                self.frame_buffer.append(frame)
                ir_spectrogram = self.frame_buffer
                data_dict = {'frame': frame,
                             'baseband_frame': frame,
                             'clutter_removal_frame': frame,
                             'clutter_removal_baseband_frame': frame,
                             'ir_spectrogram': np.array(list(ir_spectrogram))}
                self.signal_data.emit(data_dict)  # notify the uwb data for the sensor tab




    def start_sensor(self, device_name, min_range, max_range, center_frequency, fps, baseband):
        if self.xeThruX4Sensor_interface:
            self.xeThruX4Sensor_interface.config_x4_sensor(device_name=device_name, min_range=min_range,
                                                           max_range=max_range,
                                                           center_frequency=center_frequency, FPS=fps,
                                                           baseband=baseband)
            self.xeThruX4Sensor_interface.clear_xep_buffer()
        else:  # simulation mode
            print('Start Simulating Xe4Thru data')

        self.ir_spectrogram = list()  # reset spectrogram
        self.center_frequency = center_frequency
        self._is_running = True

    def stop_sensor(self):
        self._is_running = False
        self.xeThruX4Sensor_interface.stop_sensor() if self.xeThruX4Sensor_interface else print('Stopping simulating')

