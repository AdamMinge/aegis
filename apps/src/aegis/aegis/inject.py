import subprocess
import pyinjector

from aegis.client import Client


def attach_to_process(pid: int, library: str) -> Client | None:
    try:
        pyinjector.inject(pid, library)
    except pyinjector.InjectorError:
        return None

    return Client()


def start_new_process(application: str, library: str) -> Client | None:
    try:
        process = subprocess.Popen([application])
    except OSError:
        return None

    return attach_to_process(process.pid, library)
