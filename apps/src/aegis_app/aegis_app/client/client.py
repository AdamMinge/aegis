import grpc
import time


from PySide6.QtCore import QObject, Signal
from PySide6.QtNetwork import QHostAddress

from aegis.aegis_pb2_grpc import RecorderStub, MarkerStub, ObjectStub, SnifferStub


class ClientException(Exception):
    def __init__(self, error_str: str):
        self._error_str = error_str

    def __str__(self):
        return self._error_str


class Client(QObject):
    connected = Signal()
    disconnected = Signal()

    def __init__(self):
        super().__init__()
        self._connection_state = grpc.ChannelConnectivity.IDLE

    def connect_to_host(self, host: QHostAddress, port: int):
        self._channel = grpc.insecure_channel(f"{host.toString()}:{port}")
        self._channel.subscribe(self._on_channel_state_change, try_to_connect=True)

        self.recorder_stub = RecorderStub(self._channel)
        self.marker_stub = MarkerStub(self._channel)
        self.sniffer_stub = SnifferStub(self._channel)
        self.object_stub = ObjectStub(self._channel)

    def close(self):
        self._channel.close()

    def wait_for_connected(self, timeout: float) -> bool:
        start_time = time.time()

        while time.time() - start_time < timeout:
            if self._connection_state == grpc.ChannelConnectivity.READY:
                return True
            time.sleep(0.1)

        return False

    def is_connected(self) -> bool:
        return self._connection_state == grpc.ChannelConnectivity.READY

    def _on_channel_state_change(self, new_state):
        if self._connection_state == new_state:
            return

        old_state = self._connection_state
        self._connection_state = new_state

        if old_state == grpc.ChannelConnectivity.READY:
            self.disconnected.emit()
        if new_state == grpc.ChannelConnectivity.READY:
            self.connected.emit()