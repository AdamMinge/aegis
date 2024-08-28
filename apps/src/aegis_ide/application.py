from PySide6.QtWidgets import QApplication, QMainWindow


class Application(QApplication):
    def __init__(self, argv):
        super().__init__(argv)


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.__init_ui()

    def __init_ui(self):
        self.setWindowTitle("Aegis IDE")
        self.setGeometry(100, 100, 800, 600)
