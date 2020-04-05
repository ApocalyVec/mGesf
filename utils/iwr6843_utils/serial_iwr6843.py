import time

import serial

from mGesf.exceptions import DataPortNotOpenError, BufferOverFlowError
from utils.iwr6843_utils.parse_tlv import decode_iwr_tlv

data_timeout = 0.000015  # timeout for 921600 baud; 0.00000868055 for a byte


def serialConfig(configFileName, dataPortName, userPortName):
    # Open the serial ports for the configuration and the data ports
    try:
        cliPort = serial.Serial(userPortName,
                                115200)  # CLI port cannot have timeout because the stream is user-programmed
        dataPort = serial.Serial(dataPortName, 921600, timeout=data_timeout)
    except serial.SerialException as se:
        raise Exception('serial_iwr6843.serialConfig: Serial Port Occupied, error = ' + str(se))

    # Read the configuration file and send it to the board
    config = [line.rstrip('\r\n') for line in open(configFileName)]
    for line in config:
        cliPort.write((line + '\r').encode())
        time.sleep(0.01)

    time.sleep(1)
    cli_result = cliPort.read(cliPort.in_waiting).decode()
    print(cli_result)  # CLI output of the board

    return cliPort, dataPort


def sensor_start(cli_port):
    cli_port.write(('sensorStart 0\n').encode())
    result = cli_port.read(cli_port.in_waiting).decode()

    print(result)


def sensor_stop(cli_port):
    cli_port.write(('sensorStop\n').encode())
    result = cli_port.read(cli_port.in_waiting).decode()

    print(result)


def close_connection(user_port, data_port):
    user_port.reset_input_buffer()
    user_port.reset_output_buffer()

    data_port.reset_input_buffer()
    data_port.reset_output_buffer()

    user_port.close()
    data_port.close()


def clear_serial_buffer(user_port, data_port):
    user_port.reset_input_buffer()
    user_port.reset_output_buffer()

    data_port.reset_input_buffer()
    data_port.reset_output_buffer()

