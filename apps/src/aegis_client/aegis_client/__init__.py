import os

AEGIS_CLIENT_PORT = int(os.environ.get("AEGIS_CLIENT_PORT", 5010))
AEGIS_CLIENT_DLL = str(os.environ.get("AEGIS_CLIENT_DLL"))

from aegis_client.application import Application, MainWindow

__all__ = [
    "Application",
    "MainWindow",
]
