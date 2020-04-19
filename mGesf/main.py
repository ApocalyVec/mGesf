import sys

from PyQt5.QtWidgets import QApplication

from mGesf.main_window import MainWindow
from utils.iwr6843_utils.mmWave_interface import MmWaveSensorInterface


if __name__ == '__main__':
    '''
    User defined variables
    '''
    # path to which you wish to save the recorded data
    data_path = 'D:/data/mGesF'
    # AoP configs
    # configFileName = 'profiles/aop/profile_rp_basic.cfg'  # use your config file
    # configFileName = 'profiles/aop/2fps_rdHeatmap.cfg'
    # configFileName = 'profiles/aop/3fps_rd_heatmap.cfg'
    # configFileName = 'profiles/aop/25fps_rdHeatmap.cfg'
    # configFileName = 'profiles/aop/30fps_rdHeatmap.cfg'
    configFileName = 'profiles/aop/aop_azitest_withRD.cfg'
    configFileName = 'profiles/aop/aop_azitest_withoutRD.cfg'


    # ISK configs
    # configFileName = 'profiles/isk/2D/3fps_rd_heatmap.cfg'
    # configFileName = 'profiles/isk/2D/5fps_rd_heatmap.cfg'
    # configFileName = 'profiles/isk/2D/15fps_rd_heatmap.cfg'
    # configFileName = 'profiles/isk/2D/30fps_rd_heatmap_clutterRemoved.cfg'
    # configFileName = 'profiles/isk/2D/30fps_rd_heatmap.cfg'
    # configFileName = 'profiles/isk/2D/25fps_rd_heatmap.cfg'

    # configFileName = 'profiles/isk/3D/5fps_rd.cfg'
    # number of range bins for mmWave sensor
    num_range_bin = 8

    # ports for AoP
    # dataPortName = 'COM8'  # set this to your standard port (data)
    # userPortName = 'COM9'  # set this to your enhanced port (user)

    '''
    # ports for ISK
    dataPortName = 'COM9'  # set this to your data port
    userPortName = 'COM8'  # set this to your user port
    '''

    '''
    Start of the application script (do not change this part unless you know what you're doing)
    '''
    # setup connection to IWR6843
    _mmw_interface = MmWaveSensorInterface(num_range_bin=num_range_bin)
    # _mmw_interface = None

    # setup system constants
    refresh = 1  # refresh_interval every x ms
    app = QApplication(sys.argv)
    window = MainWindow(mmw_interface=_mmw_interface, refresh_interval=refresh, data_path=data_path)
    app.exec_()
    print('Resuming Console Interaction.')
