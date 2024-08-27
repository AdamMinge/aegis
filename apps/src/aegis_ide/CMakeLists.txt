# ----------------------------------------------------------------------- #
# ---------------------------- Set CMake Version ------------------------ #
# ----------------------------------------------------------------------- #
cmake_minimum_required(VERSION 3.11)
# ----------------------------------------------------------------------- #
# -------------------------- Find external libraries -------------------- #
# ----------------------------------------------------------------------- #
find_package(
  Python3
  COMPONENTS Interpreter
  REQUIRED)
# ----------------------------------------------------------------------- #
# ---------------------------- Create Virtual Env ----------------------- #
# ----------------------------------------------------------------------- #
set(app_aegis_ide_venv_path ${CMAKE_CURRENT_BINARY_DIR}/venv)

if(WIN32)
  set(app_aegis_ide_python_exe ${app_aegis_ide_venv_path}/Scripts/python.exe)
  set(app_aegis_ide_pyside6_rcc_exe
      ${app_aegis_ide_venv_path}/Scripts/pyside6-rcc.exe)
else()
  set(app_aegis_ide_python_exe ${app_aegis_ide_venv_path}/bin/python)
  set(app_aegis_ide_pyside6_rcc_exe ${app_aegis_ide_venv_path}/bin/pyside6-rcc)
endif()

add_custom_target(
  app_aegis_ide_create_virtualenv
  COMMAND ${CMAKE_COMMAND} -E make_directory ${app_aegis_ide_venv_path}
  COMMAND ${Python3_EXECUTABLE} -m venv ${app_aegis_ide_venv_path}
  COMMENT "Creating Aegis IDE virtual environment"
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
# ----------------------------------------------------------------------- #
# --------------------------- Install Requirements ---------------------- #
# ----------------------------------------------------------------------- #
set(app_aegis_ide_requirements_file
    ${CMAKE_SOURCE_DIR}/apps/src/aegis_ide/requirements.txt)

add_custom_target(
  app_aegis_ide_install_requirements
  COMMAND ${app_aegis_ide_python_exe} -m pip install --upgrade pip
  COMMAND ${app_aegis_ide_python_exe} -m pip install -r
          ${app_aegis_ide_requirements_file}
  COMMENT "Installing Aegis IDE requirements from requirements.txt"
  DEPENDS app_aegis_ide_create_virtualenv)
# ----------------------------------------------------------------------- #
# -------------------------------- Build RCC ---------------------------- #
# ----------------------------------------------------------------------- #
set(app_aegis_ide_qrc_input
    ${CMAKE_SOURCE_DIR}/apps/resource/aegis_ide/aegis_ide.qrc)
set(app_aegis_ide_qrc_output ${CMAKE_CURRENT_SOURCE_DIR}/__generated__/rcc.py)

add_custom_target(
  app_aegis_ide_build_rcc
  COMMAND ${app_aegis_ide_pyside6_rcc_exe} ${app_aegis_ide_qrc_input} -o
          ${app_aegis_ide_qrc_output}
  COMMENT "Build Aegis IDE resources"
  DEPENDS app_aegis_ide_install_requirements)
# ----------------------------------------------------------------------- #
# ---------------------------------- Setup ------------------------------ #
# ----------------------------------------------------------------------- #
add_custom_target(
  app_aegis_ide_setup ALL
  COMMENT
    "Setting up Aegis IDE (creating venv, installing dependencies and build rcc)"
  DEPENDS app_aegis_ide_install_requirements app_aegis_ide_build_rcc)
# ----------------------------------------------------------------------- #
# --------------------------------- Execute ----------------------------- #
# ----------------------------------------------------------------------- #
add_custom_target(
  app_aegis_ide_execution
  COMMAND ${client_python_exe} ${CMAKE_SOURCE_DIR}/apps/aegis_ide/main.py
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/apps/aegis
  COMMENT "Running Aegis IDE using virtual environment")

add_dependencies(app_aegis_ide_execution app_aegis_ide_install_requirements)
# ----------------------------------------------------------------------- #
