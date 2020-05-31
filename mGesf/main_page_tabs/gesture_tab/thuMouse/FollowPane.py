from PyQt5 import QtGui, QtCore
from PyQt5.QtWidgets import QLabel, QHBoxLayout, QWidget
from mGesf.main_page_tabs.gesture_tab.thuMouse.Target import Target
from random import randint
import config

# To ensure that the cursor does not touch the new target
margin = 5


class FollowPane(QWidget):
    """
    The follow canvas
    """

    def __init__(self, parent):
        """
        :type parent: QHBoxLayout
        """
        super().__init__()

        # initialize the target
        self.target = Target()

        # add to parent
        self.parent = parent
        self.parent.addWidget(self.target)

        self.target_diameter = max(self.target.height(), self.target.width())

        # starting point
        self.target_position_x = int(self.width() / 2)
        self.target_position_y = int(self.height() / 2)

        # move the target to the initial point
        self.target.move(self.target_position_x, self.target_position_y)

        print(self.target_position_x, self.target_position_y)
        # how big a step does the circle move?
        self.step = 3

    def activate(self):
        self.target.turn_on()

    def move_target(self, cursor_x, cursor_y):
        """
        Avoids the cursor
        :param cursor_x: x position of the cursor
        :param cursor_y: y position of the cursor
        :return:
        """
        new_x1 = randint(min(self.target_diameter + margin, cursor_x - self.target_diameter - margin),
                         max(self.target_diameter + margin, cursor_x - self.target_diameter - margin),)
        new_x2 = randint(min(self.width() - self.target_diameter - margin, cursor_x + margin),
                         max(self.width() - self.target_diameter - margin, cursor_x + margin))

        new_xs = [new_x1, new_x2]

        new_y1 = randint(min(self.target_diameter + margin, cursor_y - self.target_diameter - margin),
                         max(self.target_diameter + margin, cursor_y - self.target_diameter - margin))
        new_y2 = randint(min(cursor_y + margin, self.width() - self.target_diameter - margin),
                         max(cursor_y + margin, self.width() - self.target_diameter - margin))
        new_ys = [new_y1, new_y2]

        # choose one x and one y
        new_x = new_xs[randint(0, 1)]
        new_y = new_ys[randint(0, 1)]

        self.target.move(new_x, new_y)
