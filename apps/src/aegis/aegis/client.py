import pyinjector
import subprocess
import grpc
import time
import os

from PySide6.QtCore import QObject, Signal
from PySide6.QtNetwork import QHostAddress

from aegis.aegis_pb2_grpc import RecorderStub, SnifferStub, ObjectStub


class ClientException(Exception):
    def __init__(self, error_str: str):
        self._error_str = error_str

    def __str__(self):
        return self._error_str


class Client(QObject):
    __WAIT_FOR_CONNECTED_TIMEOUT = 3.0
    __WAIT_FOR_PROCESS_TIMEOUT = 0.5

    connected = Signal()
    disconnected = Signal()

    @classmethod
    def attach_to_existing_process(
        cls, host: QHostAddress, port: int, pid: int, library: str
    ) -> "Client":
        try:
            pyinjector.inject(pid, library)
        except pyinjector.InjectorError as e:
            raise ClientException(str(e))

        client = cls()
        client.connect_to_host(host, port)
        if not client.wait_for_connected(cls.__WAIT_FOR_CONNECTED_TIMEOUT):
            raise ClientException(
                f"Connection failed to {host.toString()}:{port} after waiting for {cls.__WAIT_FOR_CONNECTED_TIMEOUT} ms"
            )

        return client

    @classmethod
    def attach_to_new_process(
        cls, host: QHostAddress, port: int, app: str, library: str
    ) -> "Client":
        try:
            process = subprocess.Popen([app], env=os.environ)
            time.sleep(cls.__WAIT_FOR_PROCESS_TIMEOUT)
        except OSError as e:
            raise ClientException(str(e))

        return cls.attach_to_existing_process(host, port, process.pid, library)

    def __init__(self):
        super().__init__()
        self._connection_state = grpc.ChannelConnectivity.IDLE

    def connect_to_host(self, host: QHostAddress, port: int):
        self._channel = grpc.insecure_channel(f"{host.toString()}:{port}")
        self._channel.subscribe(self._on_channel_state_change, try_to_connect=True)

        self._recorder_stub = RecorderStub(self._channel)
        self._sniffer_stub = SnifferStub(self._channel)
        self._object_stub = ObjectStub(self._channel)

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
