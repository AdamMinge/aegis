import subprocess
import pyinjector
import typing

from PySide6.QtCore import QObject, Slot, Signal
from PySide6.QtNetwork import QTcpSocket, QHostAddress


class Client(QTcpSocket):
    @staticmethod
    def attach_to_existing_process(
        host: QHostAddress, port: int, pid: int, library: str
    ) -> typing.Optional["Client"]:
        try:
            pyinjector.inject(pid, library)
        except pyinjector.InjectorError:
            return None

        client = Client()
        client.connectToHost(host, port)
        if not client.waitForConnected(3000):
            return None

        return client

    @staticmethod
    def attach_to_new_process(
        host: QHostAddress, port: int, app: str, library: str
    ) -> typing.Optional["Client"]:
        try:
            process = subprocess.Popen([app])
        except OSError:
            return None

        return Client.attach_to_existing_process(host, port, process.pid, library)

    def __init__(self):
        super().__init__()
