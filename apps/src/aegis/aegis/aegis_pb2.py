# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# NO CHECKED-IN PROTOBUF GENCODE
# source: aegis/aegis.proto
# Protobuf Python Version: 5.27.2
"""Generated protocol buffer code."""
from google.protobuf import descriptor as _descriptor
from google.protobuf import descriptor_pool as _descriptor_pool
from google.protobuf import runtime_version as _runtime_version
from google.protobuf import symbol_database as _symbol_database
from google.protobuf.internal import builder as _builder
_runtime_version.ValidateProtobufRuntimeVersion(
    _runtime_version.Domain.PUBLIC,
    5,
    27,
    2,
    '',
    'aegis/aegis.proto'
)
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()


from google.protobuf import empty_pb2 as google_dot_protobuf_dot_empty__pb2
from google.protobuf import struct_pb2 as google_dot_protobuf_dot_struct__pb2


DESCRIPTOR = _descriptor_pool.Default().AddSerializedFile(b'\n\x11\x61\x65gis/aegis.proto\x12\x0b\x61\x65gis_proto\x1a\x1bgoogle/protobuf/empty.proto\x1a\x1cgoogle/protobuf/struct.proto\"\"\n\x0fRecorderCommand\x12\x0f\n\x07\x63ommand\x18\x01 \x01(\t\".\n\x0eOptionalObject\x12\x12\n\x05query\x18\x01 \x01(\tH\x00\x88\x01\x01\x42\x08\n\x06_query\"\x17\n\x06Object\x12\r\n\x05query\x18\x01 \x01(\t\"/\n\x07Objects\x12$\n\x07objects\x18\x01 \x03(\x0b\x32\x13.aegis_proto.Object\"4\n\nObjectTree\x12&\n\x05nodes\x18\x01 \x03(\x0b\x32\x17.aegis_proto.ObjectNode\"Y\n\nObjectNode\x12#\n\x06object\x18\x01 \x01(\x0b\x32\x13.aegis_proto.Object\x12&\n\x05nodes\x18\x02 \x03(\x0b\x32\x17.aegis_proto.ObjectNode\"l\n\nMethodCall\x12#\n\x06object\x18\x01 \x01(\x0b\x32\x13.aegis_proto.Object\x12\x0e\n\x06method\x18\x02 \x01(\t\x12)\n\targuments\x18\x03 \x03(\x0b\x32\x16.google.protobuf.Value\"n\n\x0ePropertyUpdate\x12#\n\x06object\x18\x01 \x01(\x0b\x32\x13.aegis_proto.Object\x12\x10\n\x08property\x18\x02 \x01(\t\x12%\n\x05value\x18\x03 \x01(\x0b\x32\x16.google.protobuf.Value\"\x1a\n\x07Methods\x12\x0f\n\x07methods\x18\x01 \x03(\t\"<\n\nProperties\x12.\n\nproperties\x18\x01 \x03(\x0b\x32\x1a.aegis_proto.PropertyValue\"H\n\rPropertyValue\x12\x10\n\x08property\x18\x01 \x01(\t\x12%\n\x05value\x18\x02 \x01(\x0b\x32\x16.google.protobuf.Value\"\xdb\x01\n\x0cObjectChange\x12)\n\x05\x61\x64\x64\x65\x64\x18\x01 \x01(\x0b\x32\x18.aegis_proto.ObjectAddedH\x00\x12-\n\x07removed\x18\x02 \x01(\x0b\x32\x1a.aegis_proto.ObjectRemovedH\x00\x12\x33\n\nreparented\x18\x03 \x01(\x0b\x32\x1d.aegis_proto.ObjectReparentedH\x00\x12-\n\x07renamed\x18\x04 \x01(\x0b\x32\x1a.aegis_proto.ObjectRenamedH\x00\x42\r\n\x0b\x63hange_type\"W\n\x0bObjectAdded\x12#\n\x06object\x18\x01 \x01(\x0b\x32\x13.aegis_proto.Object\x12#\n\x06parent\x18\x02 \x01(\x0b\x32\x13.aegis_proto.Object\"4\n\rObjectRemoved\x12#\n\x06object\x18\x01 \x01(\x0b\x32\x13.aegis_proto.Object\"\\\n\x10ObjectReparented\x12#\n\x06object\x18\x01 \x01(\x0b\x32\x13.aegis_proto.Object\x12#\n\x06parent\x18\x02 \x01(\x0b\x32\x13.aegis_proto.Object\"a\n\rObjectRenamed\x12\'\n\nold_object\x18\x01 \x01(\x0b\x32\x13.aegis_proto.Object\x12\'\n\nnew_object\x18\x02 \x01(\x0b\x32\x13.aegis_proto.Object2\x84\x01\n\rMarkerService\x12\x39\n\x05Start\x12\x16.google.protobuf.Empty\x1a\x16.google.protobuf.Empty\"\x00\x12\x38\n\x04Stop\x12\x16.google.protobuf.Empty\x1a\x16.google.protobuf.Empty\"\x00\x32]\n\x0fRecorderService\x12J\n\x0eListenCommands\x12\x16.google.protobuf.Empty\x1a\x1c.aegis_proto.RecorderCommand\"\x00\x30\x01\x32\xd0\x04\n\rObjectService\x12\x41\n\x07GetTree\x12\x1b.aegis_proto.OptionalObject\x1a\x17.aegis_proto.ObjectTree\"\x00\x12\x33\n\x04\x46ind\x12\x13.aegis_proto.Object\x1a\x14.aegis_proto.Objects\"\x00\x12\x37\n\tGetParent\x12\x13.aegis_proto.Object\x1a\x13.aegis_proto.Object\"\x00\x12:\n\x0bGetChildren\x12\x13.aegis_proto.Object\x1a\x14.aegis_proto.Objects\"\x00\x12?\n\nCallMethod\x12\x17.aegis_proto.MethodCall\x1a\x16.google.protobuf.Empty\"\x00\x12G\n\x0eUpdateProperty\x12\x1b.aegis_proto.PropertyUpdate\x1a\x16.google.protobuf.Empty\"\x00\x12\x39\n\nGetMethods\x12\x13.aegis_proto.Object\x1a\x14.aegis_proto.Methods\"\x00\x12?\n\rGetProperties\x12\x13.aegis_proto.Object\x1a\x17.aegis_proto.Properties\"\x00\x12L\n\x13ListenObjectChanges\x12\x16.google.protobuf.Empty\x1a\x19.aegis_proto.ObjectChange\"\x00\x30\x01\x62\x06proto3')

_globals = globals()
_builder.BuildMessageAndEnumDescriptors(DESCRIPTOR, _globals)
_builder.BuildTopDescriptorsAndMessages(DESCRIPTOR, 'aegis.aegis_pb2', _globals)
if not _descriptor._USE_C_DESCRIPTORS:
  DESCRIPTOR._loaded_options = None
  _globals['_RECORDERCOMMAND']._serialized_start=93
  _globals['_RECORDERCOMMAND']._serialized_end=127
  _globals['_OPTIONALOBJECT']._serialized_start=129
  _globals['_OPTIONALOBJECT']._serialized_end=175
  _globals['_OBJECT']._serialized_start=177
  _globals['_OBJECT']._serialized_end=200
  _globals['_OBJECTS']._serialized_start=202
  _globals['_OBJECTS']._serialized_end=249
  _globals['_OBJECTTREE']._serialized_start=251
  _globals['_OBJECTTREE']._serialized_end=303
  _globals['_OBJECTNODE']._serialized_start=305
  _globals['_OBJECTNODE']._serialized_end=394
  _globals['_METHODCALL']._serialized_start=396
  _globals['_METHODCALL']._serialized_end=504
  _globals['_PROPERTYUPDATE']._serialized_start=506
  _globals['_PROPERTYUPDATE']._serialized_end=616
  _globals['_METHODS']._serialized_start=618
  _globals['_METHODS']._serialized_end=644
  _globals['_PROPERTIES']._serialized_start=646
  _globals['_PROPERTIES']._serialized_end=706
  _globals['_PROPERTYVALUE']._serialized_start=708
  _globals['_PROPERTYVALUE']._serialized_end=780
  _globals['_OBJECTCHANGE']._serialized_start=783
  _globals['_OBJECTCHANGE']._serialized_end=1002
  _globals['_OBJECTADDED']._serialized_start=1004
  _globals['_OBJECTADDED']._serialized_end=1091
  _globals['_OBJECTREMOVED']._serialized_start=1093
  _globals['_OBJECTREMOVED']._serialized_end=1145
  _globals['_OBJECTREPARENTED']._serialized_start=1147
  _globals['_OBJECTREPARENTED']._serialized_end=1239
  _globals['_OBJECTRENAMED']._serialized_start=1241
  _globals['_OBJECTRENAMED']._serialized_end=1338
  _globals['_MARKERSERVICE']._serialized_start=1341
  _globals['_MARKERSERVICE']._serialized_end=1473
  _globals['_RECORDERSERVICE']._serialized_start=1475
  _globals['_RECORDERSERVICE']._serialized_end=1568
  _globals['_OBJECTSERVICE']._serialized_start=1571
  _globals['_OBJECTSERVICE']._serialized_end=2163
# @@protoc_insertion_point(module_scope)
