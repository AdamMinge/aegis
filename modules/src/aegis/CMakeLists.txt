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

set(sources
    ${source_root}/client.cpp
    ${source_root}/server.cpp
    ${source_root}/sniffer.cpp
    ${source_root}/serializer.cpp
    ${source_root}/manager.cpp
    ${source_root}/command/children.cpp
    ${source_root}/command/command.cpp
    ${source_root}/command/dump_methods.cpp
    ${source_root}/command/dump_properties.cpp
    ${source_root}/command/executor.cpp
    ${source_root}/command/find.cpp
    ${source_root}/command/help.cpp
    ${source_root}/command/invoke_method.cpp
    ${source_root}/command/parent.cpp
    ${source_root}/command/record.cpp
    ${source_root}/command/set_property.cpp
    ${source_root}/search/searcher.cpp
    ${source_root}/search/strategy.cpp
    ${source_root}/record/recorder.cpp
    ${source_root}/record/strategy.cpp)

set(includes
    ${include_root}/export.h
    ${include_root}/client.h
    ${include_root}/server.h
    ${include_root}/sniffer.h
    ${include_root}/serializer.h
    ${include_root}/response.h
    ${include_root}/manager.h
    ${include_root}/command/children.h
    ${include_root}/command/command.h
    ${include_root}/command/dump_methods.h
    ${include_root}/command/dump_properties.h
    ${include_root}/command/executor.h
    ${include_root}/command/find.h
    ${include_root}/command/help.h
    ${include_root}/command/invoke_method.h
    ${include_root}/command/parent.h
    ${include_root}/command/record.h
    ${include_root}/command/set_property.h
    ${include_root}/search/searcher.h
    ${include_root}/search/strategy.h
    ${include_root}/record/recorder.h
    ${include_root}/record/strategy.h
    ${include_root}/record/action.h)

set(resources ${resource_root}/aegis.qrc)
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
find_package(qtils REQUIRED COMPONENTS serializer dpi)
# ----------------------------------------------------------------------- #
# -------------------------- Create plugin shared lib ------------------- #
# ----------------------------------------------------------------------- #
aegis_add_module(
  aegis
  SOURCES
  ${sources}
  ${includes}
  ${resources}
  DEPENDS
  Qt6::Core
  Qt6::Widgets
  Qt6::Network
  qtils::serializer
  qtils::dpi)

target_include_directories(
  aegis
  PUBLIC $<BUILD_INTERFACE:${AEGIS_SOURCE_DIR}/modules/include>
  INTERFACE $<INSTALL_INTERFACE:include>
  PRIVATE ${AEGIS_SOURCE_DIR}/modules/src)
# ----------------------------------------------------------------------- #
