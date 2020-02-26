import sys

from PyQt5.QtWidgets import QApplication

from mGesf.main_window import MainWindow
from utils.iwr6843_utils.mmWave_interface import MmWaveSensorInterface


if __name__ == '__main__':
    '''
    User defined variables
    '''
    # AoP configs
    # configFileName = 'profiles/aop/profile_rp_basic.cfg'  # use your config file
    # configFileName = 'profiles/aop/profile_rdh_clutterRemoved.cfg'  # use your config file

    # ISK configs
    # configFileName = 'profiles/isk/2D/3fps_rd_heatmap.cfg'
    # configFileName = 'profiles/isk/2D/5fps_rd_heatmap.cfg'
    # configFileName = 'profiles/isk/2D/15fps_rd_heatmap.cfg'
    # configFileName = 'profiles/isk/2D/30fps_rd_heatmap_clutterRemoved.cfg'
    configFileName = 'profiles/isk/2D/30fps_rd_heatmap.cfg'

    # configFileName = 'profiles/isk/3D/5fps_rd.cfg'

    '''
    # ports for AoP
    dataPortName = 'COM14'  # set this to your standard port (data)
    userPortName = 'COM3'  # set this to your enhanced port (user)
    '''

    # ports for ISK
    dataPortName = 'COM9'  # set this to your data port
    userPortName = 'COM8'  # set this to your user port

    '''
    Start of the application script (do not change this part unless you know what you're doing)
    '''
    # setup connection to IWR6843
    _mmw_interface = MmWaveSensorInterface(configFileName, data_port=dataPortName, user_port=userPortName)
    # _mmw_interface = None

    # setup system constants
    refresh = 1  # refresh_interval every x ms
    app = QApplication(sys.argv)
    window = MainWindow(mmw_interface=_mmw_interface, refresh_interval=refresh)
    app.exec_()
    print('Resuming Console Interaction.')
