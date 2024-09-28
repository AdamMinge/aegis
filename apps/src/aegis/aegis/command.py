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

        parser_recorder = subparsers.add_parser("recorder")
        parser_recorder.add_argument(
            "action",
            choices=["start", "stop", "clear", "report"],
            help="Start, stop, clear and get report of recorder",
        )
        parser_recorder.set_defaults(func=self._execute_recorder)

        parser_find = subparsers.add_parser("find")
        parser_find.add_argument(
            "object", type=str, help="The object query used to finding objects"
        )
        parser_find.set_defaults(func=self._execute_find)

        parser_parent = subparsers.add_parser("parent")
        parser_parent.add_argument(
            "object", type=str, help="The object query used to finding objects"
        )
        parser_parent.set_defaults(func=self._execute_parent)

        parser_children = subparsers.add_parser("children")
        parser_children.add_argument(
            "object", type=str, help="The object query used to finding objects"
        )
        parser_children.set_defaults(func=self._execute_children)

        parser_invoke_method = subparsers.add_parser("invoke_method")
        parser_invoke_method.set_defaults(func=self._execute_invoke_method)

        parser_set_property = subparsers.add_parser("set_property")
        parser_set_property.set_defaults(func=self._execute_set_property)

        parser_dump_methods = subparsers.add_parser("dump_methods")
        parser_dump_methods.set_defaults(func=self._execute_dump_methods)

        parser_dump_properties = subparsers.add_parser("dump_properties")
        parser_dump_properties.set_defaults(func=self._execute_dump_properties)

    def _execute_sniffer(self, args) -> empty_pb2.Empty:
        if args.action == "start":
            return self._sniffer_stub.Start(empty_pb2.Empty())
        elif args.action == "stop":
            return self._sniffer_stub.Stop(empty_pb2.Empty())

        return empty_pb2.Empty()

    def _execute_recorder(self, args) -> empty_pb2.Empty | aegis_pb2.ReportResponse:
        if args.action == "start":
            return self._recorder_stub.Start(empty_pb2.Empty())
        elif args.action == "stop":
            return self._recorder_stub.Stop(empty_pb2.Empty())
        elif args.action == "clear":
            return self._recorder_stub.Clear(empty_pb2.Empty())
        elif args.action == "report":
            return self._recorder_stub.Report(empty_pb2.Empty())

        return empty_pb2.Empty()

    def _execute_find(self, args) -> empty_pb2.Empty:
        return self._object_stub.Find(aegis_pb2.ObjectRequest(object=args.object))

    def _execute_parent(self, args) -> empty_pb2.Empty:
        return self._object_stub.Parent(aegis_pb2.ObjectRequest(object=args.object))

    def _execute_children(self, args) -> empty_pb2.Empty:
        return self._object_stub.Children(aegis_pb2.ObjectRequest(object=args.object))

    def _execute_invoke_method(self, args) -> empty_pb2.Empty:
        return empty_pb2.Empty()

    def _execute_set_property(self, args) -> empty_pb2.Empty:
        return empty_pb2.Empty()

    def _execute_dump_methods(self, args) -> empty_pb2.Empty:
        return empty_pb2.Empty()

    def _execute_dump_properties(self, args) -> empty_pb2.Empty:
        return empty_pb2.Empty()
