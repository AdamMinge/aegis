import subprocess
import pyinjector
import time

from PySide6.QtNetwork import QTcpSocket, QHostAddress


class ClientException(Exception):
    def __init__(self, error_str: str):
        self.error_str = error_str

    def __str__(self):
        return self.error_str


class Client(QTcpSocket):
    @staticmethod
    def attach_to_existing_process(
        host: QHostAddress, port: int, pid: int, library: str
    ) -> "Client":
        try:
            pyinjector.inject(pid, library)
        except pyinjector.InjectorError as e:
            raise ClientException(str(e))

        client = Client()
        client.connectToHost(host, port)
        if not client.waitForConnected(1000):
            raise ClientException(client.errorString())

        return client

    @staticmethod
    def attach_to_new_process(
        host: QHostAddress, port: int, app: str, library: str
    ) -> "Client":
        try:
            process = subprocess.Popen([app])
            time.sleep(1)
        except OSError as e:
            raise ClientException(str(e))

        return Client.attach_to_existing_process(host, port, process.pid, library)

    def __init__(self):
        super().__init__()
