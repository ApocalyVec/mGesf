import time

from mGesf.exceptions import BufferOverFlowError
from utils.iwr6843_utils import serial_iwr6843


class MmWaveSensorInterface:

    def __init__(self, config, data_port, user_port, *args, **kwargs):
        self.uport, self.dport = serial_iwr6843.serialConfig(config, dataPortName=data_port,
                                                             userPortName=user_port)
        serial_iwr6843.clear_serial_buffer(self.uport, self.dport)
        print('mmw Interface: Booting up sensor ...')
        time.sleep(2)
        print('mmw Interface: Done!')

        # constants
        self.data_chunk_size = 32  # this MUST be 32 for TLV to work without magic number
        self.data_buffer_max_size = 3200
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
        time.sleep(2)
        print('mmw Interface: started!')

    def process_frame(self):
        detected_points = None
        while detected_points is None:
            try:
                detected_points, range_profile, rd_heatmap = serial_iwr6843.parse_stream(self.dport)
            except BufferOverFlowError:
                print('Python buffer overflows, closing sensor connection')
                self.stop_sensor()
                time.sleep(1)
                print('Sensor stopped, raising keyboardInterrupt')
                raise KeyboardInterrupt
        return detected_points, range_profile, rd_heatmap

    def stop_sensor(self):
        print('mmw Interface: Stopping sensor ...')
        serial_iwr6843.sensor_stop(self.uport)
        time.sleep(1)
        serial_iwr6843.close_connection(self.uport, self.dport)
        print('mmw Interface: stopped!')
