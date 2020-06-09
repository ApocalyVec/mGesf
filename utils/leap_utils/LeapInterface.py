import random

class LeapInterface:
    def __init__(self):
        pass

    def connect_sensor(self):
        # connect to the sensor
        pass

    def start_sensor(self):
        # tell the sensor to start sending frames
        pass

    def process_frame(self):
        # return a frame of the sensor
        # this function should return NONE WITHOUT blocking if a frame is not complete
        return random.random()

    def sensor_stop(self):
        pass