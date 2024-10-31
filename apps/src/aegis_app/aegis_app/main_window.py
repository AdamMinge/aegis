from PySide6.QtWidgets import (
    QMainWindow,
    QSizePolicy,
)
from PySide6.QtCore import Slot

from aegis_app.client import Client


class MainWindow(QMainWindow):
    def __init__(self, client: Client):
        super().__init__()
        self._init_ui()

    def _init_ui(self):
        self.setWindowTitle("Aegis App")
        self.resize(800, 600)
        self.setSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.Preferred)
