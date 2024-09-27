import argparse
import typing
import io

from google.protobuf import empty_pb2
from aegis import aegis_pb2
from aegis.client import Client, ClientException


class CommandParserException(Exception):
    def __init__(self, error_str: str):
        self._error_str = error_str

    def __str__(self):
        return self._error_str


class CommandParserRequestedCommand(CommandParserException):
    pass


class CommandParserInternalError(CommandParserException):
    pass


def raise_instead_of_print(method):
    def wrapper(self, file=None):
        if file is None:
            file = io.StringIO()
        method(self, file)
        result = file.getvalue()
        if isinstance(file, io.StringIO):
            file.close()
        raise CommandParserRequestedCommand(result)

    return wrapper


class CommandParser(argparse.ArgumentParser):
    def __init__(self, **kwargs):
        if "prog" not in kwargs:
            kwargs["prog"] = ""
        super().__init__(exit_on_error=False, **kwargs)

    def execute(self, command: str):
        try:
            args = self.parse_args(command.split())
            return args.func(args)
        except CommandParserRequestedCommand as e:
            return str(e)

    def exit(self):
        pass

    def error(self, message):
        raise CommandParserInternalError(f"error: {message}")

    @raise_instead_of_print
    def print_help(self, file=None):
        super().print_help(file)

    @raise_instead_of_print
    def print_usage(self, file=None):
        super().print_usage(file)


class CommandClient(Client):
    def __init__(self):
        super().__init__()
        self._init_parser()

    def execute(self, command: str) -> typing.Any:
        try:
            return self._parser.execute(command)
        except Exception as e:
            raise ClientException(str(e))

    def _init_parser(self):
        self._parser = CommandParser()
        subparsers = self._parser.add_subparsers(required=True)

        parser_sniffer = subparsers.add_parser("sniffer")
        parser_sniffer.add_argument(
            "action", choices=["start", "stop"], help="Start or stop the sniffer"
        )
        parser_sniffer.set_defaults(func=self._execute_sniffer)

    def _execute_sniffer(self, args) -> empty_pb2.Empty:
        if args.action == "start":
            self._sniffer_stub.Start(empty_pb2.Empty())
        elif args.action == "stop":
            self._sniffer_stub.Stop(empty_pb2.Empty())

        return empty_pb2.Empty()
