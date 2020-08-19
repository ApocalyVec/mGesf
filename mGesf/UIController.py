"""
Adopted from https://www.tautvidas.com/blog/2018/02/automating-basic-tasks-in-games-with-opencv-and-python/
Date: 04/08/2019
"""

import time
from pynput.mouse import Button, Controller as MouseController


class UIController:
    def __init__(self):
        self.mouse = MouseController()

    def get_mouse_position(self):
        return self.mouse.position

    def displace_mouse(self, xdisp, ydisp):
        new_x = self.mouse.position[0] + xdisp
        new_y = self.mouse.position[1] + ydisp
        self.set_mouse_position(new_x, new_y)

    def move_mouse_stepwise(self, x, y):
        """
        Smoothly move the curvor to (x, y)
        :param x:
        :param y:
        :return: new mouse xy position
        """
        return self.smooth_move_mouse(
            self.mouse.position[0],
            self.mouse.position[1],
            x,
            y)

    def set_mouse_position(self, x, y):
        self.mouse.position = (int(x), int(y))

    def smooth_move_mouse(self, from_x, from_y, to_x, to_y, speed=0.2):
        steps = 40
        sleep_per_step = speed // steps
        x_delta = (to_x - from_x) / steps
        y_delta = (to_y - from_y) / steps
        for step in range(steps):
            new_x = x_delta * (step + 1) + from_x
            new_y = y_delta * (step + 1) + from_y
            self.set_mouse_position(new_x, new_y)
            time.sleep(sleep_per_step)
        return new_x, new_y

    def left_mouse_click(self):
        self.mouse.click(Button.left)