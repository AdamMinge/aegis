import sys

from aegis_client.__generated__ import rcc
from aegis_client.application import Application, MainWindow


def main():
    app = Application(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())


if __name__ == "__main__":
    main()
