import os

from PySide6.QtNetwork import QHostAddress

AEGIS_SERVER_HOST = QHostAddress(os.environ.get("AEGIS_SERVER_HOST", "127.0.0.1"))
AEGIS_SERVER_PORT = int(os.environ.get("AEGIS_SERVER_HOST", "5010"))
AEGIS_SERVER_DLL = str(os.environ.get("AEGIS_SERVER_DLL"))
