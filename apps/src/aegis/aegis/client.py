import subprocess
import pyinjector
import typing
import enum
import time
import json
import os

from PySide6.QtCore import QObject, Signal
from PySide6.QtNetwork import QTcpSocket, QHostAddress


class ClientResponse:
    class Level(enum.Enum):
        Success = "success"
        Warning = "warning"
        Error = "error"

    def __init__(self, level: Level, body: typing.Any = None):
        self.level = level
        self.body = body


class ClientException(Exception):
    def __init__(self, error_str: str):
        self.error_str = error_str

    def __str__(self):
        return self.error_str


class Client(QObject):
    closed = Signal()

    @staticmethod
    def attach_to_existing_process(
        host: QHostAddress, port: int, pid: int, library: str
    ) -> "Client":
        try:
            pyinjector.inject(pid, library, True)
        except pyinjector.InjectorError as e:
            raise ClientException(str(e))

        socket = QTcpSocket()
        socket.connectToHost(host, port)
        if not socket.waitForConnected(1000):
            raise ClientException(socket.errorString())

        return Client(socket)

    @staticmethod
    def attach_to_new_process(
        host: QHostAddress, port: int, app: str, library: str
    ) -> "Client":
        try:
            process = subprocess.Popen([app], env=os.environ)
            time.sleep(0.5)
        except OSError as e:
            raise ClientException(str(e))

        return Client.attach_to_existing_process(host, port, process.pid, library)

    def __init__(self, socket: QTcpSocket):
        super().__init__()
        self.__socket = socket
        self.__socket.disconnected.connect(self.closed)

    def close(self):
        self.__socket.close()

    def send(self, command: str) -> ClientResponse:
        self.__socket.write(command.encode("utf-8"))

        if self.__socket.waitForReadyRead(2000):
            data = self.__socket.readAll().data().decode("utf-8")  # type: ignore
            try:
                json_data = json.loads(data)

                match json_data["status"]:
                    case ClientResponse.Level.Success.value:
                        level = ClientResponse.Level.Success
                    case ClientResponse.Level.Warning.value:
                        level = ClientResponse.Level.Warning
                    case ClientResponse.Level.Error.value:
                        level = ClientResponse.Level.Error
                body = json_data.get("body", None)

                return ClientResponse(level, body)
            except json.JSONDecodeError as e:
                return ClientResponse(
                    ClientResponse.Level.Error, f"Failed to decode response: {e}"
                )
        else:
            return ClientResponse(
                ClientResponse.Level.Error, "Timeout waiting for response"
            )
