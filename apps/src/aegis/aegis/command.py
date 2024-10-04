import argparse
import typing
import io

from google.protobuf import empty_pb2, struct_pb2
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

    @property
    def _parser_class(self):
        pass


class Command:
    def __init__(self, client: Client):
        self._client = client

    def register(self, parser):
        raise NotImplementedError("Subclasses should implement this method.")


class SnifferCommand(Command):
    def register(self, parser):
        parser_sniffer = parser.add_parser("sniffer")
        parser_sniffer.add_argument(
            "-a",
            "--action",
            choices=["start", "stop"],
            required=True,
            help="Start or stop the sniffer",
        )
        parser_sniffer.set_defaults(func=self._execute)

    def _execute(self, args):
        if args.action == "start":
            return self._client._sniffer_stub.Start(empty_pb2.Empty())  # type: ignore
        elif args.action == "stop":
            return self._client._sniffer_stub.Stop(empty_pb2.Empty())  # type: ignore

        return empty_pb2.Empty()


class RecorderCommand(Command):
    def register(self, parser):
        parser_recorder = parser.add_parser("recorder")
        parser_recorder.add_argument(
            "-a",
            "--action",
            choices=["start", "stop", "clear", "report"],
            required=True,
            help="Start, stop, clear and get report of recorder",
        )
        parser_recorder.set_defaults(func=self._execute)

    def _execute(self, args):
        if args.action == "start":
            return self._client._recorder_stub.Start(empty_pb2.Empty())  # type: ignore
        elif args.action == "stop":
            return self._client._recorder_stub.Stop(empty_pb2.Empty())  # type: ignore
        elif args.action == "clear":
            return self._client._recorder_stub.Clear(empty_pb2.Empty())  # type: ignore
        elif args.action == "report":
            return self._client._recorder_stub.Report(empty_pb2.Empty())  # type: ignore

        return empty_pb2.Empty()


class FindCommand(Command):
    def register(self, parser):
        parser_find = parser.add_parser("find")
        parser_find.add_argument(
            "-o",
            "--object",
            type=str,
            required=True,
            help="The object query used to finding objects",
        )
        parser_find.set_defaults(func=self._execute)

    def _execute(self, args):
        return self._client._object_stub.Find(
            aegis_pb2.ObjectRequest(object=args.object)  # type: ignore
        )


class ParentCommand(Command):
    def register(self, parser):
        parser_parent = parser.add_parser("parent")
        parser_parent.add_argument(
            "-o",
            "--object",
            type=str,
            required=True,
            help="The object query used to finding objects",
        )
        parser_parent.set_defaults(func=self._execute)

    def _execute(self, args):
        return self._client._object_stub.Parent(
            aegis_pb2.ObjectRequest(object=args.object)  # type: ignore
        )


class ChildrenCommand(Command):
    def register(self, parser):
        parser_children = parser.add_parser("children")
        parser_children.add_argument(
            "-o",
            "--object",
            type=str,
            required=True,
            help="The object query used to finding objects",
        )
        parser_children.set_defaults(func=self._execute)

    def _execute(self, args):
        return self._client._object_stub.Children(
            aegis_pb2.ObjectRequest(object=args.object)  # type: ignore
        )


class InvokeMethodCommand(Command):
    def register(self, parser):
        parser_invoke_method = parser.add_parser("invoke_method")
        parser_invoke_method.add_argument(
            "-o",
            "--object",
            type=str,
            required=True,
            help="The object query used to finding objects",
        )
        parser_invoke_method.add_argument(
            "-m",
            "--method",
            type=str,
            required=True,
            help="The method to call",
        )
        parser_invoke_method.add_argument(
            "-a",
            "--arguments",
            type=str,
            nargs="*",
            help="The arguments used as call parameters",
        )
        parser_invoke_method.set_defaults(func=self._execute)

    def _execute(self, args):
        arguments = (
            [struct_pb2.Value(string_value=arg) for arg in args.arguments]
            if args.arguments
            else []
        )

        return self._client._object_stub.InvokeMethod(  # type: ignore
            aegis_pb2.InvokeMethodRequest(  # type: ignore
                object=args.object, method=args.method, arguments=arguments
            )
        )


class SetPropertyCommand(Command):
    def register(self, parser):
        parser_set_property = parser.add_parser("set_property")
        parser_set_property.add_argument(
            "-o",
            "--object",
            type=str,
            required=True,
            help="The object query used to finding objects",
        )
        parser_set_property.add_argument(
            "-p",
            "--property",
            type=str,
            required=True,
            help="The Property to set",
        )
        parser_set_property.add_argument(
            "-v",
            "--value",
            type=str,
            required=True,
            help="The property value to set",
        )
        parser_set_property.set_defaults(func=self._execute)

    def _execute(self, args):
        return self._client._object_stub.SetProperty(  # type: ignore
            aegis_pb2.SetPropertyRequest(  # type: ignore
                object=args.object,
                method=args.method,
                value=struct_pb2.Value(string_value=args.value),
            )
        )


class DumpMethodsCommand(Command):
    def register(self, parser):
        parser_dump_methods = parser.add_parser("dump_methods")
        parser_dump_methods.add_argument(
            "-o",
            "--object",
            type=str,
            required=True,
            help="The object query used to finding objects",
        )
        parser_dump_methods.set_defaults(func=self._execute)

    def _execute(self, args):
        return self._client._object_stub.DumpMethods(
            aegis_pb2.ObjectRequest(object=args.object)  # type: ignore
        )


class DumpPropertiesCommand(Command):
    def register(self, parser):
        parser_dump_properties = parser.add_parser("dump_properties")
        parser_dump_properties.add_argument(
            "-o",
            "--object",
            type=str,
            required=True,
            help="The object query used to finding objects",
        )
        parser_dump_properties.set_defaults(func=self._execute)

    def _execute(self, args):
        return self._client._object_stub.DumpProperties(
            aegis_pb2.ObjectRequest(object=args.object)  # type: ignore
        )


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

        self.commands = [
            SnifferCommand(self),
            RecorderCommand(self),
            FindCommand(self),
            ParentCommand(self),
            ChildrenCommand(self),
            InvokeMethodCommand(self),
            SetPropertyCommand(self),
            DumpMethodsCommand(self),
            DumpPropertiesCommand(self),
        ]

        for command in self.commands:
            command.register(subparsers)
