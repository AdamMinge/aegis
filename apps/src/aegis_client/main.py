import sys

from __generated__ import rcc

from application import Application, MainWindow


def main():
    app = Application(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
