from PySide6.QtWidgets import QWidget
from PySide6.QtCore import Signal


class ClientPage(QWidget):
    detached = Signal()

    def __init__(self):
        super().__init__()
