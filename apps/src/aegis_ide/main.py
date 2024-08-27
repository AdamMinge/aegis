import sys

from .main_window import MainWindow
from .application import Application


def main():
    app = Application(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
