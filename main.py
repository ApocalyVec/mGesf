import sys

from PyQt5.QtWidgets import QApplication

from main_window import MainWindow
from utils.iwr6843_utils.mmWave_interface import MmWaveSensorInterface


if __name__ == '__main__':
    '''
    User defined variables
    '''
    configFileName = 'profiles/20fps_04RR_14VR_12CT_8DT.cfg'  # use your config file
    dataPortName = 'COM8'  # set this to your standard port
    userPortName = 'COM9'  # set this to your enhanced port

    '''
    Start of the application script (do not change this part unless you know what you're doing)
    '''
    # setup connection to IWR6843
    _mmw_interface = MmWaveSensorInterface(configFileName, data_port=dataPortName, user_port=userPortName)
    # __mmw_interface = None

    # setup system constants
    refresh = 10  # refresh_interval every x ms
    app = QApplication(sys.argv)
    window = MainWindow(mmw_interface=_mmw_interface, refresh_interval=refresh)
    app.exec_()
