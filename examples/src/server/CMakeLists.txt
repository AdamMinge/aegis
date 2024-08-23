# ----------------------------------------------------------------------- #
# ---------------------------- Set CMake Version ------------------------ #
# ----------------------------------------------------------------------- #
cmake_minimum_required(VERSION 3.11)
# ----------------------------------------------------------------------- #
# ---------------------------- Set files variable ----------------------- #
# ----------------------------------------------------------------------- #
set(source_root ${PILLAR_SOURCE_DIR}/examples/src/server)
set(include_root ${PILLAR_SOURCE_DIR}/examples/include/server)
set(ui_root ${PILLAR_SOURCE_DIR}/examples/ui/server)
set(resource_root ${PILLAR_SOURCE_DIR}/examples/resource/server)

set(sources ${source_root}/main.cpp ${source_root}/application.cpp
            ${source_root}/main_window.cpp)

set(includes ${include_root}/application.h ${include_root}/main_window.h)

set(uis)

set(resources ${resource_root}/server.qrc)
# ----------------------------------------------------------------------- #
# ------------------------- Qt libraries configuration ------------------ #
# ----------------------------------------------------------------------- #
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS ${ui_root})
# ----------------------------------------------------------------------- #
# -------------------------- Find external libraries -------------------- #
# ----------------------------------------------------------------------- #
find_package(Qt6 REQUIRED COMPONENTS Widgets)
# ----------------------------------------------------------------------- #
# ---------------------------- Create app editor ------------------------ #
# ----------------------------------------------------------------------- #
aegis_add_example(
  example_server
  SOURCES
  ${sources}
  ${includes}
  ${uis}
  ${resources}
  DEPENDS
  Qt6::Widgets
  aegis::aegis)

target_include_directories(
  example_server
  PUBLIC $<BUILD_INTERFACE:${include_root}>
  INTERFACE $<INSTALL_INTERFACE:include>
  PRIVATE ${source_root})
# ----------------------------------------------------------------------- #
