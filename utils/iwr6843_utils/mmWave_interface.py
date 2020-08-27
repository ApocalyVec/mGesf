import time

import serial

from mGesf.exceptions import BufferOverFlowError, DataPortNotOpenError, GeneralMmWError, PortsNotSetUpError
from utils.data_utils import clutter_removal
from utils.iwr6843_utils import serial_iwr6843
from utils.iwr6843_utils.parse_tlv import decode_iwr_tlv


class MmWaveSensorInterface:

    def __init__(self, num_range_bin, buffer_size=16000, uport=None, dport=None, *args, **kwargs):
        self.uport = uport
        self.dport = dport
        # constants
        self.data_chunk_size = 32  # this MUST be 32 for TLV to work without magic number
        self.buffer_size = buffer_size
        self.num_range_bin = num_range_bin  # TODO parse this value down to the tlv decoder
        # data fields
        self.data_buffer = b''

        # clutter removal paramters
        self.rd_clutter = None
        self.ra_clutter = None
        self.rd_signal_clutter_ratio = 0.5
        self.ra_signal_clutter_ratio = 0.5


    def send_config(self, config_path):
        try:
            serial_iwr6843.serial_config(config_path, cli_port=self.uport)
            serial_iwr6843.clear_serial_buffer(self.uport, self.dport)
        except serial.serialutil.SerialException:
            print('mmw Interface: connect the sensor before sending configuration file')
            return
        print('mmw Interface: sending config and booting up sensor')
        time.sleep(2)
        print('mmw Interface: Done!')

    def start_sensor(self):
        """
        In the current implementation, after starting the sensor, you must call parse_stream
        to resolve the incoming data flow. If waiting for too long without parsing the stream,
        the data_buffer will overflow and result in an error. The maximum buffer size is 3200 bytes.
        """
        print('mmw Interface: Starting sensor ...')
        try:
            serial_iwr6843.sensor_start(self.uport)
        except AttributeError as e:
            if type(e) == AttributeError:
                raise PortsNotSetUpError
        time.sleep(1)
        print('mmw Interface: started!')

    def process_frame(self):
        detected_points, range_profile, rd_heatmap, azi_heatmap, rd_heatmap_clutter_removed, azi_heatmap_clutter_removed = None, None, None, None, None, None
        while detected_points is None and range_profile is None and rd_heatmap is None and azi_heatmap is None:
            try:
                detected_points, range_profile, rd_heatmap, azi_heatmap = self.parse_stream()
                if rd_heatmap is not None and azi_heatmap is not None:
                    rd_heatmap_clutter_removed, self.rd_clutter = clutter_removal(cur_frame=rd_heatmap,
                                                                                  clutter=self.rd_clutter,
                                                                                  signal_clutter_ratio=self.rd_signal_clutter_ratio)
                    azi_heatmap_clutter_removed, self.ra_clutter = clutter_removal(cur_frame=azi_heatmap,
                                                                                   clutter=self.ra_clutter,
                                                                                   signal_clutter_ratio=self.ra_signal_clutter_ratio)
            except (BufferOverFlowError, DataPortNotOpenError, GeneralMmWError) as e:
                print(str(e))
                if type(e) == DataPortNotOpenError:
                    print('Sensor is disconnected during reading of data, killing.')
                    raise KeyboardInterrupt
                if type(e) == BufferOverFlowError:
                    print('This is not supposed to happen in the current implementation, please report the issue')
                elif type(e) == GeneralMmWError:
                    print('An unknown error occurred, closing sensor connection, , please report the issue')

                print('closing sensor connection because of an error')
                self.stop_sensor()
                time.sleep(1)
                print('Sensor stopped, raising keyboardInterrupt, printing TLV buffer for debug')
                print(self.data_buffer)

        return detected_points, range_profile, rd_heatmap, azi_heatmap, rd_heatmap_clutter_removed, azi_heatmap_clutter_removed

    def stop_sensor(self):
        print('mmw Interface: Stopping sensor ...')
        try:
            serial_iwr6843.sensor_stop(self.uport)
        except AttributeError as e:
            if type(e) == AttributeError:
                raise PortsNotSetUpError
        print('mmw Interface: stopped!')

    def connect(self, uport_ame, dport_name):
        data_timeout = 0.000015  # timeout for 921600 baud; 0.00000868055 for a byte
        try:
            self.uport = serial.Serial(uport_ame,
                                       115200)  # CLI port cannot have timeout because the stream is user-programmed
            self.dport = serial.Serial(dport_name, 921600, timeout=data_timeout)
        except serial.SerialException as se:
            raise Exception('serial_iwr6843.serialConfig: Serial Port Occupied, error = ' + str(se))

    def close_connection(self):
        print('mmw Interface: Stopping sensor ...')
        serial_iwr6843.sensor_stop(self.uport)  # stop the sensor before closing the connection
        print('mmw Interface: stopped!')
        time.sleep(1)
        serial_iwr6843.close_connection(self.uport, self.dport)
        print('mmw Interface: sensor connection closed')
        self.uport = None
        self.dport = None

    def is_connected(self):
        return self.uport is not None and self.dport is not None

    def parse_stream(self):
        """

        :param data_port:
        :return: will be None if the data packet is not complete yet
        """

        try:
            self.data_buffer += self.dport.read(self.data_chunk_size)

            if len(self.data_buffer) > self.buffer_size:
                raise BufferOverFlowError

            is_packet_complete, leftover_data, detected_points, range_profile, rd_heatmap, azi_heatmap = \
                decode_iwr_tlv(self.data_buffer)

            if is_packet_complete:
                self.data_buffer = b'' + leftover_data
                return detected_points, range_profile, rd_heatmap, azi_heatmap
            else:
                return None, None, None, None
        except (serial.serialutil.SerialException, AttributeError, TypeError, ValueError) as e:
            if type(e) == serial.serialutil.SerialException:
                raise DataPortNotOpenError
            else:
                raise GeneralMmWError

    def set_rd_csr(self, value):
        self.rd_signal_clutter_ratio = value
        self.rd_clutter = None

    def set_ra_csr(self, value):
        self.ra_signal_clutter_ratio = value
        self.ra_clutter = None