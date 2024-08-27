from PySide6.QtWidgets import QWidget
from PySide6.QtCore import Signal


class AttachToProcessPage(QWidget):
    attached = Signal(int)

    def __init__(self):
        super().__init__()
