import sys

from PyQt5.QtWidgets import QApplication

from mGesf.main_window import MainWindow
from utils.decaWave_utils.DecaUWB_interface import UWBSensorInterface
from utils.iwr6843_utils.mmWave_interface import MmWaveSensorInterface
from utils.decaWave_utils import DecaUWB_interface

from utils.leap_utils.LeapInterface import LeapInterface

if __name__ == '__main__':
    '''
    User defined variables
    '''
    # path to which you wish to save the recorded data
    data_path = '/Users/neneko/desktop/testData'
    # AoP configs
    # configFileName = 'profiles/aop/profile_rp_basic.cfg'  # use your config file
    # configFileName = 'profiles/aop/2fps_rdHeatmap.cfg'
    # configFileName = 'profiles/aop/3fps_rd_heatmap.cfg'
    # configFileName = 'profiles/aop/25fps_rdHeatmap.cfg'
    # configFileName = 'profiles/aop/30fps_rdHeatmap.cfg'
    # configFileName = 'profiles/aop/aop_azitest_withRD.cfg'
    # configFileName = 'profiles/aop/aop_azitest_withoutRD.cfg'
    # configFileName = 'mGesf/profiles/aop/30fps_azi_rd.cfg'
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
    # _mmw_interface = MmWaveSensorInterface(num_range_bin=num_range_bin)
    _leap_interface = LeapInterface()
    _mmw_interface = None


    '''
    UWB demo interface
    '''
    # _uwb_interface_anchor = UWBSensorInterface('Anchor', 520)
    # _uwb_interface_tag = UWBSensorInterface('Tag', 520)

    _uwb_interface_anchor = None
    _uwb_interface_tag = None

    _uwb_interface_anchor = UWBSensorInterface('Anchor', 520)
    _uwb_interface_tag = UWBSensorInterface('Tag', 520)
    # try:
    #     _uwb_interface_anchor = UWBSensorInterface('Anchor',520)
    #     _uwb_interface_tag = UWBSensorInterface('Tag', 520)
    #
    # except:
    #     _uwb_interface_anchor = None
    #     _uwb_interface_tag = None

    # _uwb_interface_anchor.connect_virtual_port('COM32')
    # _uwb_interface_tag.connect_virtual_port('COM30')



    #TODO _uwb_interface = DecaUWBInterface(framerate=, exe_path=, uport=)
    # _uwb_interface_anchor = None
    #_uwb_interface_tag = None

    # setup system constants
    # refresh_interval every x ms, use 33 when in simulation mode, use 1 when connected to sensors
    refresh = 1

    app = QApplication(sys.argv)
    window = MainWindow(mmw_interface=_mmw_interface, leap_interface=_leap_interface,
                        uwb_interface_anchor=_uwb_interface_anchor, uwb_interface_tag=_uwb_interface_tag,
                        refresh_interval=refresh, data_path=data_path)
    app.exec_()
    print('Resuming Console Interaction.')
