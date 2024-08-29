from PySide6.QtWidgets import QWidget
from PySide6.QtCore import Signal


class ClientEditor(QWidget):
    detached = Signal()

    def __init__(self):
        super().__init__()
