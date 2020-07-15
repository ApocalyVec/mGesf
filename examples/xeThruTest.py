from utils.XeThru_utils.xeThruX4_interface import xeThruX4SensorInterface
from utils.XeThru_utils.xeThruX4_algorithm import *
import matplotlib.pyplot



if __name__ == '__main__':
    X4M300 = xeThruX4SensorInterface()

    X4M300.config_x4_sensor('COM8', max_range=0.6, FPS=1)

    X4M300.clear_xep_buffer()
    counter = 0
    while 1:
        frame = X4M300.read_frame()
        if frame is not None:

            counter += 1
            print(frame)
            plt.plot(frame)
            plt.show()

