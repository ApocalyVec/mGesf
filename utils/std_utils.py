from PyQt5 import QtGui, QtCore


class Stream(QtCore.QObject):
    newText = QtCore.pyqtSignal(str)

    def write(self, text):
        if text != '\n':
            self.newText.emit(str(text))