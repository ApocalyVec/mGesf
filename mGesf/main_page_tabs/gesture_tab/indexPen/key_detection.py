from PyQt5 import QtGui, QtCore


def on_key(key, tab):
    # test for a specific key
    if key == QtCore.Qt.Key_Return or key == QtCore.Qt.Key_Enter:
        tab.set_KeyPressed_True()

    else:
        tab.set_KeyPressed_False()


class ReturnKeyDetectionWidget(QtGui.QWidget):
    keyPressed = QtCore.pyqtSignal(int)

    def keyPressEvent(self, event):
        super(ReturnKeyDetectionWidget, self).keyPressEvent(event)
        self.keyPressed.emit(event.key())
