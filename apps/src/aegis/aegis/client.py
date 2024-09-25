import pyinjector
import subprocess
import grpc
import time
import os

from PySide6.QtCore import QObject, Signal
from PySide6.QtNetwork import QHostAddress

from aegis.aegis_pb2_grpc import RecorderStub, SnifferStub, ObjectStub

WAIT_FOR_CONNECTED_TIMEOUT = 3.0
WAIT_FOR_PROCESS_TIMEOUT = 0.5


class ClientException(Exception):
    def __init__(self, error_str: str):
        self.error_str = error_str

    def __str__(self):
        return self.error_str


class Client(QObject):
    connected = Signal()
    disconnected = Signal()

    def __init__(self):
        super().__init__()
        self.__connection_state = grpc.ChannelConnectivity.IDLE

    def connect_to_host(self, host: QHostAddress, port: int):
        self.__channel = grpc.insecure_channel(f"{host.toString()}:{port}")
        self.__channel.subscribe(self.__on_channel_state_change, try_to_connect=True)

        self.__recorder_stub = RecorderStub(self.__channel)
        self.__sniffer_stub = SnifferStub(self.__channel)
        self.__object_stub = ObjectStub(self.__channel)

    def close(self):
        self.__channel.close()

    def wait_for_connected(self, timeout: int) -> bool:
        start_time = time.time()

        while time.time() - start_time < timeout:
            if self.__connection_state == grpc.ChannelConnectivity.READY:
                return True
            time.sleep(0.1)

        return False

    def is_connected(self) -> bool:
        return self.__connection_state == grpc.ChannelConnectivity.READY

    def __on_channel_state_change(self, new_state):
        if self.__connection_state == new_state:
            return

        old_state = self.__connection_state
        self.__connection_state = new_state

        if old_state == grpc.ChannelConnectivity.READY:
            self.disconnected.emit()
        if new_state == grpc.ChannelConnectivity.READY:
            self.connected.emit()


def attach_client_to_existing_process(
    host: QHostAddress, port: int, pid: int, library: str
) -> Client:
    try:
        pyinjector.inject(pid, library)
    except pyinjector.InjectorError as e:
        raise ClientException(str(e))

    client = Client()
    client.connect_to_host(host, port)
    if not client.wait_for_connected(WAIT_FOR_CONNECTED_TIMEOUT):
        raise ClientException(
            f"Connection failed to {host.toString()}:{port} after waiting for {WAIT_FOR_CONNECTED_TIMEOUT} ms"
        )

    return client


def attach_client_to_new_process(
    host: QHostAddress, port: int, app: str, library: str
) -> Client:
    try:
        process = subprocess.Popen([app], env=os.environ)
        time.sleep(WAIT_FOR_PROCESS_TIMEOUT)
    except OSError as e:
        raise ClientException(str(e))

    return attach_client_to_existing_process(host, port, process.pid, library)
