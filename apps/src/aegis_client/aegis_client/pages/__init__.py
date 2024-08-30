import os

AEGIS_CLIENT_PORT = int(os.environ.get("AEGIS_CLIENT_PORT", 5010))
AEGIS_CLIENT_DLL = str(os.environ.get("AEGIS_CLIENT_DLL"))

from .attach_to_process import AttachToProcess
from .client_editor import ClientEditor
from .injection_mode_selector import InjectionModeSelector
from .start_new_process import StartNewProcess


__all__ = [
    "AttachToProcess",
    "ClientEditor",
    "InjectionModeSelector",
    "StartNewProcess",
]
