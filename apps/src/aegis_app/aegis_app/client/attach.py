import pyinjector
import subprocess
import time
import os

from PySide6.QtNetwork import QHostAddress

from aegis_app.client import Client


CONNECTING_TIMEOUT = 0.5
ATTACHING_TIMEOUT = 0.5


class AttachException(Exception):
    def __init__(self, error_str: str):
        self._error_str = error_str

    def __str__(self):
        return self._error_str


def attach_to_existing_process(
    host: QHostAddress, port: int, pid: int, library: str
) -> Client:
    try:
        pyinjector.inject(pid, library)
    except pyinjector.InjectorError as e:
        raise AttachException(str(e))

    client = Client()
    client.connect_to_host(host, port)
    if not client.wait_for_connected(CONNECTING_TIMEOUT):
        raise AttachException(
            f"Connection failed to {host.toString()}:{port} after waiting for {CONNECTING_TIMEOUT} ms"
        )

    return client


def attach_to_new_process(
    host: QHostAddress, port: int, app: str, library: str
) -> Client:
    try:
        process = subprocess.Popen([app], env=os.environ)
        time.sleep(ATTACHING_TIMEOUT)
    except OSError as e:
        raise AttachException(str(e))

    return attach_to_existing_process(host, port, process.pid, library)
