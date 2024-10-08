# ----------------------------------------------------------------------- #
# ---------------------------- Set CMake Version ------------------------ #
# ----------------------------------------------------------------------- #
cmake_minimum_required(VERSION 3.11)
# ----------------------------------------------------------------------- #
# ---------------------------- Set files variable ----------------------- #
# ----------------------------------------------------------------------- #
set(source_root ${AEGIS_SOURCE_DIR}/modules/src/aegis)
set(include_root ${AEGIS_SOURCE_DIR}/modules/include/aegis)
set(resource_root ${AEGIS_SOURCE_DIR}/modules/resource/aegis)
set(proto_root ${AEGIS_SOURCE_DIR}/modules/proto/aegis)

set(sources
    ${source_root}/inject.cpp
    ${source_root}/module.cpp
    ${source_root}/server/server.cpp
    ${source_root}/server/call.cpp
    ${source_root}/service/marker.cpp
    ${source_root}/service/recorder.cpp
    ${source_root}/service/object.cpp
    ${source_root}/service/sniffer.cpp
    ${source_root}/record/action.cpp
    ${source_root}/record/recorder.cpp
    ${source_root}/record/strategy.cpp
    ${source_root}/search/query.cpp
    ${source_root}/search/searcher.cpp
    ${source_root}/search/strategy.cpp
    ${source_root}/mark/marker.cpp
    ${source_root}/mark/widget_marker.cpp
    ${source_root}/mark/widget_tooltip.cpp
    ${source_root}/sniff/sniffer.cpp)

set(includes
    ${include_root}/config.h
    ${include_root}/export.h
    ${include_root}/module.h
    ${include_root}/server/service.h
    ${include_root}/server/server.h
    ${include_root}/server/call.h
    ${include_root}/service/marker.h
    ${include_root}/service/recorder.h
    ${include_root}/service/object.h
    ${include_root}/service/sniffer.h
    ${include_root}/record/action.h
    ${include_root}/record/recorder.h
    ${include_root}/record/strategy.h
    ${include_root}/search/query.h
    ${include_root}/search/searcher.h
    ${include_root}/search/strategy.h
    ${include_root}/mark/marker.h
    ${include_root}/mark/widget_marker.h
    ${include_root}/mark/widget_tooltip.h
    ${include_root}/sniff/sniffer.h)

set(resources ${resource_root}/aegis.qrc)

set(protos ${proto_root}/aegis.proto)
# ----------------------------------------------------------------------- #
# ------------------------- Qt libraries configuration ------------------ #
# ----------------------------------------------------------------------- #
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
# ----------------------------------------------------------------------- #
# -------------------------- Find external libraries -------------------- #
# ----------------------------------------------------------------------- #
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)
# ----------------------------------------------------------------------- #
# -------------------------- Create plugin shared lib ------------------- #
# ----------------------------------------------------------------------- #
aegis_add_module(
  aegis
  SOURCES
  ${sources}
  ${includes}
  ${resources}
  PROTOS
  ${protos}
  DEPENDS
  Qt6::Core
  Qt6::Widgets
  Qt6::Network)

target_include_directories(
  aegis
  PUBLIC $<BUILD_INTERFACE:${AEGIS_SOURCE_DIR}/modules/include>
  INTERFACE $<INSTALL_INTERFACE:include>
  PRIVATE ${AEGIS_SOURCE_DIR}/modules/src)
# ----------------------------------------------------------------------- #
