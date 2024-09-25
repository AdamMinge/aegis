import sys

from aegis_console import rcc
from aegis_console.application import Application, MainWindow


def main():
    app = Application(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
