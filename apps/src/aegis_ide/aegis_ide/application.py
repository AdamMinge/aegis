from PySide6.QtWidgets import (
    QApplication,
    QMainWindow,
    QSizePolicy,
)


class Application(QApplication):
    def __init__(self, argv):
        super().__init__(argv)


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self._init_ui()

    def _init_ui(self):
        self.setWindowTitle("Aegis IDE")
        self.resize(800, 600)
        self.setSizePolicy(QSizePolicy.Policy.Preferred, QSizePolicy.Policy.Preferred)
