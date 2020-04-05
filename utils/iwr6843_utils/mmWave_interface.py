import time

import serial

from mGesf.exceptions import BufferOverFlowError, DataPortNotOpenError
from utils.iwr6843_utils import serial_iwr6843
from utils.iwr6843_utils.parse_tlv import decode_iwr_tlv


class MmWaveSensorInterface:

    def __init__(self, config, data_port, user_port, num_range_bin, buffer_size=3200, *args, **kwargs):
        self.uport, self.dport = serial_iwr6843.serialConfig(config, dataPortName=data_port,
                                                             userPortName=user_port)
        serial_iwr6843.clear_serial_buffer(self.uport, self.dport)
        print('mmw Interface: Booting up sensor ...')
        time.sleep(2)
        print('mmw Interface: Done!')

        # constants
        self.data_chunk_size = 32  # this MUST be 32 for TLV to work without magic number
        self.buffer_size = buffer_size
        self.num_range_bin = num_range_bin
        # data fields
        self.data_buffer = b''

    def start_sensor(self):
        """
        In the current implementation, after starting the sensor, you must call parse_stream
        to resolve the incoming data flow. If waiting for too long without parsing the stream,
        the data_buffer will overflow and result in an error. The maximum buffer size is 3200 bytes.
        """
        print('mmw Interface: Starting sensor ...')
        serial_iwr6843.sensor_start(self.uport)
        time.sleep(1)
        print('mmw Interface: started!')

    def process_frame(self):
        detected_points = None
        while detected_points is None:
            try:
                detected_points, range_profile, rd_heatmap = self.parse_stream()
            except (BufferOverFlowError, DataPortNotOpenError) as e:
                print(str(e))
                print('An error occured, closing sensor connection')
                self.stop_sensor()
                time.sleep(1)
                print('Sensor stopped, raising keyboardInterrupt')
                raise KeyboardInterrupt
        return detected_points, range_profile, rd_heatmap

    def stop_sensor(self):
        print('mmw Interface: Stopping sensor ...')
        serial_iwr6843.sensor_stop(self.uport)
        print('mmw Interface: stopped!')

    def close_connection(self):
        print('mmw Interface: Stopping sensor ...')
        serial_iwr6843.sensor_stop(self.uport)  # stop the sensor before closing the connection
        print('mmw Interface: stopped!')
        time.sleep(1)
        serial_iwr6843.close_connection(self.uport, self.dport)
        print('mmw Interface: sensor connection closed')

    def parse_stream(self):
        """

        :param data_port:
        :return: will be None if the data packet is not complete yet
        """

        try:
            self.data_buffer += self.dport.read(self.data_chunk_size)

            if len(self.data_buffer) > self.buffer_size:
                print(self.data_buffer)
                raise BufferOverFlowError

            is_packet_complete, leftover_data, detected_points, range_profile, rd_heatmap = \
                decode_iwr_tlv(self.data_buffer)

            if is_packet_complete:
                self.data_buffer = b'' + leftover_data
                return detected_points, range_profile, rd_heatmap
            else:
                return None, None, None
        except (serial.serialutil.SerialException, AttributeError, TypeError):
            raise DataPortNotOpenError
