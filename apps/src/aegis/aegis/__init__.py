import os, sys

sys.path.append(os.path.dirname(os.path.realpath(__file__)))

from client import Client
from inject import attach_to_process, start_new_process

__all__ = [
    "Client",
    "attach_to_process",
    "start_new_process",
]
