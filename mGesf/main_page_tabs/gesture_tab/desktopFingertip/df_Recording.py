from utils.GUI_operation_tab import *


class Recording(QWidget):
    def __init__(self):
        super().__init__()
        # -------------------- First class --------------------
        self.main_page = QHBoxLayout(self)
        self.setLayout(self.main_page)

        self.show()
