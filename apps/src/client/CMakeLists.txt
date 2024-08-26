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
# --------------------------- Install Dependencies ---------------------- #
# ----------------------------------------------------------------------- #
set(client_venv_path ${CMAKE_CURRENT_BINARY_DIR}/venv)

if(WIN32)
  set(client_python_exe ${client_venv_path}/Scripts/python.exe)
else()
  set(client_python_exe ${client_venv_path}/bin/python)
endif()

add_custom_target(
  client_create_virtualenv
  COMMAND ${CMAKE_COMMAND} -E make_directory ${client_venv_path}
  COMMAND ${Python3_EXECUTABLE} -m venv ${client_venv_path}
  COMMENT "Creating Client virtual environment"
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
# ----------------------------------------------------------------------- #
# --------------------------- Install Dependencies ---------------------- #
# ----------------------------------------------------------------------- #
set(client_requirements_file
    ${CMAKE_SOURCE_DIR}/apps/src/client/requirements.txt)

add_custom_target(
  client_install_requirements
  COMMAND ${client_python_exe} -m pip install --upgrade pip
  COMMAND ${client_python_exe} -m pip install -r ${client_requirements_file}
  COMMENT "Installing Client dependencies from requirements.txt"
  DEPENDS client_create_virtualenv)
# ----------------------------------------------------------------------- #
# ------------------------------- Setup Client -------------------------- #
# ----------------------------------------------------------------------- #
add_custom_target(
  setup_client ALL
  COMMENT "Setting up Client (creating venv and installing dependencies)"
  DEPENDS client_install_requirements)
# ----------------------------------------------------------------------- #
# ------------------------------ Execute Client ------------------------- #
# ----------------------------------------------------------------------- #
add_custom_target(
  client_execution
  COMMAND ${client_python_exe} ${CMAKE_SOURCE_DIR}/apps/client/main.py
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/apps/client
  COMMENT "Running Client using virtual environment")

add_dependencies(client_execution client_install_requirements)
# ----------------------------------------------------------------------- #
