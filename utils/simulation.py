import random
import time
import numpy as np


def simulateData():
    detObj = dict()

    num_points = random.randint(1, 40)
    detObj['x'] = np.random.uniform(low=0.0, high=1.0, size=(num_points,))
    detObj['y'] = np.random.uniform(low=0.0, high=1.0, size=(num_points,))
    detObj['z'] = np.random.uniform(low=0.0, high=1.0, size=(num_points,))
    detObj['doppler'] = np.random.uniform(low=0.0, high=1.0, size=(num_points,))

    time.sleep(0.033)

    return True, 0, detObj


def sim_heatmap(shape: list):
    return np.random.random(shape).astype(np.float16)


def sim_detected_points():
    num_points = random.randint(1, 40)
    return np.transpose(np.asarray([np.random.uniform(low=0.0, high=1.0, size=(num_points,)),
                                    np.random.uniform(low=0.0, high=1.0, size=(num_points,)),
                                    np.random.uniform(low=0.0, high=1.0, size=(num_points,)),
                                    np.random.uniform(low=0.0, high=1.0, size=(num_points,))]))
