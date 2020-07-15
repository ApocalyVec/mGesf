# https://stackoverflow.com/questions/47494305/python-pyqt4-slider-with-tick-labels

from PyQt5 import QtWidgets, QtCore, QtGui
from PyQt5.QtCore import Qt, QRect, QPoint
from PyQt5.QtGui import QPainter, QFont
from PyQt5.QtWidgets import QStyle, QStyleOptionSlider


# slide Bar Range example.
# max min interval + date example.

MAXVAL = 3
MINVAL = -0.5

class RangeSlider(QtWidgets.QWidget):

    def __init__(self):
        super().__init__()

        self.minTime = 0
        self.maxTime = 0
        self.minRangeTime = 0
        self.maxRangeTime = 0

        self.sliderMin = MAXVAL
        self.sliderMax = MAXVAL

        self.setupUi(self)

    def setupUi(self, RangeSlider):
        RangeSlider.setObjectName("RangeSlider")
        # RangeSlider.resize(1000, 65)
        # RangeSlider.setMaximumSize(QtCore.QSize(16777215, 65))
        self.RangeBarVLayout = QtWidgets.QVBoxLayout(RangeSlider)
        self.RangeBarVLayout.setContentsMargins(5, 0, 5, 0)
        self.RangeBarVLayout.setSpacing(0)
        self.RangeBarVLayout.setObjectName("RangeBarVLayout")

        self.slidersFrame = QtWidgets.QFrame(RangeSlider)
        self.slidersFrame.setMaximumSize(QtCore.QSize(16777215, 25))
        self.slidersFrame.setFrameShape(QtWidgets.QFrame.StyledPanel)
        self.slidersFrame.setFrameShadow(QtWidgets.QFrame.Raised)
        self.slidersFrame.setObjectName("slidersFrame")
        self.horizontalLayout = QtWidgets.QHBoxLayout(self.slidersFrame)
        self.horizontalLayout.setSizeConstraint(QtWidgets.QLayout.SetMinimumSize)
        self.horizontalLayout.setContentsMargins(5, 2, 5, 2)
        self.horizontalLayout.setSpacing(0)
        self.horizontalLayout.setObjectName("horizontalLayout")

        ## Start Slider Widget
        self.startSlider = QtWidgets.QSlider(self.slidersFrame)
        self.startSlider.setMaximum(self.sliderMin)
        self.startSlider.setMinimumSize(QtCore.QSize(100, 5))
        self.startSlider.setMaximumSize(QtCore.QSize(16777215, 10))

        font = QtGui.QFont()
        font.setKerning(True)

        self.startSlider.setFont(font)
        self.startSlider.setAcceptDrops(False)
        self.startSlider.setAutoFillBackground(False)
        self.startSlider.setOrientation(QtCore.Qt.Horizontal)
        self.startSlider.setInvertedAppearance(True)
        self.startSlider.setObjectName("startSlider")
        self.startSlider.setValue(MAXVAL)
        self.startSlider.valueChanged.connect(self.handleStartSliderValueChange)
        self.horizontalLayout.addWidget(self.startSlider)

        ## End Slider Widget
        self.endSlider = QtWidgets.QSlider(self.slidersFrame)
        self.endSlider.setMaximum(MAXVAL)
        self.endSlider.setMinimumSize(QtCore.QSize(100, 5))
        self.endSlider.setMaximumSize(QtCore.QSize(16777215, 10))
        self.endSlider.setTracking(True)
        self.endSlider.setOrientation(QtCore.Qt.Horizontal)
        self.endSlider.setObjectName("endSlider")
        self.endSlider.setValue(self.sliderMax)
        self.endSlider.valueChanged.connect(self.handleEndSliderValueChange)

        #self.endSlider.sliderReleased.connect(self.handleEndSliderValueChange)

        self.horizontalLayout.addWidget(self.endSlider)

        self.RangeBarVLayout.addWidget(self.slidersFrame)

        #self.retranslateUi(RangeSlider)
        QtCore.QMetaObject.connectSlotsByName(RangeSlider)

        self.show()

    @QtCore.pyqtSlot(int)
    def handleStartSliderValueChange(self, value):
        self.startSlider.setValue(value)

    @QtCore.pyqtSlot(int)
    def handleEndSliderValueChange(self, value):
        self.endSlider.setValue(value)



class LabeledSlider(QtWidgets.QWidget):
    def __init__(self, minimum, maximum, interval=1, orientation=Qt.Horizontal,
                 labels=None, parent=None):
        super(LabeledSlider, self).__init__(parent=parent)

        levels = range(minimum, maximum + interval, interval)
        if labels is not None:
            if not isinstance(labels, (tuple, list)):
                raise Exception("<labels> is a list or tuple.")
            if len(labels) != len(levels):
                raise Exception("Size of <labels> doesn't match levels.")
            self.levels = list(zip(levels, labels))
        else:
            self.levels = list(zip(levels, map(str, levels)))

        if orientation == Qt.Horizontal:
            self.layout = QtWidgets.QVBoxLayout(self)
        elif orientation == Qt.Vertical:
            self.layout = QtWidgets.QHBoxLayout(self)
        else:
            raise Exception("<orientation> wrong.")

        # gives some space to print labels
        self.left_margin = 10
        self.top_margin = 10
        self.right_margin = 10
        self.bottom_margin = 10

        self.layout.setContentsMargins(self.left_margin, self.top_margin,
                                       self.right_margin, self.bottom_margin)

        self.slider = QtWidgets.QSlider(orientation, self)
        self.slider.setMinimum(minimum)
        self.slider.setMaximum(maximum)
        self.slider.setValue(minimum)
        if orientation == Qt.Horizontal:
            self.slider.setTickPosition(QtWidgets.QSlider.TicksBelow)
            self.slider.setMinimumWidth(300)  # just to make it easier to read
        else:
            self.slider.setTickPosition(QtWidgets.QSlider.TicksLeft)
            self.slider.setMinimumHeight(300)  # just to make it easier to read
        self.slider.setTickInterval(interval)
        self.slider.setSingleStep(1)

        self.layout.addWidget(self.slider)

    def paintEvent(self, e):

        super(LabeledSlider, self).paintEvent(e)

        style = self.slider.style()
        painter = QPainter(self)
        st_slider = QStyleOptionSlider()
        st_slider.initFrom(self.slider)
        st_slider.orientation = self.slider.orientation()

        length = style.pixelMetric(QStyle.PM_SliderLength, st_slider, self.slider)
        available = style.pixelMetric(QStyle.PM_SliderSpaceAvailable, st_slider, self.slider)

        for v, v_str in self.levels:

            # get the size of the label
            rect = painter.drawText(QRect(), Qt.TextDontPrint, v_str)

            if self.slider.orientation() == Qt.Horizontal:
                # I assume the offset is half the length of slider, therefore
                # + length//2
                x_loc = QStyle.sliderPositionFromValue(self.slider.minimum(),
                                                       self.slider.maximum(), v, available) + length // 2

                # left bound of the text = center - half of text width + L_margin
                left = x_loc - rect.width() // 2 + self.left_margin
                bottom = self.rect().bottom()

                # enlarge margins if clipping
                if v == self.slider.minimum():
                    if left <= 0:
                        self.left_margin = rect.width() // 2 - x_loc
                    if self.bottom_margin <= rect.height():
                        self.bottom_margin = rect.height()

                    self.layout.setContentsMargins(self.left_margin,
                                                   self.top_margin, self.right_margin,
                                                   self.bottom_margin)

                if v == self.slider.maximum() and rect.width() // 2 >= self.right_margin:
                    self.right_margin = rect.width() // 2
                    self.layout.setContentsMargins(self.left_margin,
                                                   self.top_margin, self.right_margin,
                                                   self.bottom_margin)

            else:
                y_loc = QStyle.sliderPositionFromValue(self.slider.minimum(),
                                                       self.slider.maximum(), v, available, upsideDown=True)

                bottom = y_loc + length // 2 + rect.height() // 2 + self.top_margin - 3
                # there is a 3 px offset that I can't attribute to any metric

                left = self.left_margin - rect.width()
                if left <= 0:
                    self.left_margin = rect.width() + 2
                    self.layout.setContentsMargins(self.left_margin,
                                                   self.top_margin, self.right_margin,
                                                   self.bottom_margin)

            pos = QPoint(left, bottom)
            painter.setFont(QFont("times", 12))
            painter.drawText(pos, v_str)

        return

    def setValue(self, value=0):
        self.slider.setValue(value)

