from PySide6.QtWidgets import QApplication

from . import rcc


class Application(QApplication):
    def __init__(self, argv):
        super().__init__(argv)
