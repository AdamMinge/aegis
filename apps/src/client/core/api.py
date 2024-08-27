from PySide6.QtCore import QObject, Slot, Signal
from PySide6.QtNetwork import QTcpSocket


class ClientApi(QObject):
    def __init__(self):
        self.socket = QTcpSocket(self)

    @Slot()
    def connectToHost(self, host, port) -> bool:
        self.socket.connectToHost("127.0.0.1", 12345)
        return self.socket.waitForConnected(3000)

    @Slot()
    def disconnectFromHost(self) -> bool:
        self.socket.disconnectFromHost()
        return True
