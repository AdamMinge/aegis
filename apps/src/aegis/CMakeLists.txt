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
set(app_aegis_venv_path ${CMAKE_CURRENT_BINARY_DIR}/venv)

if(WIN32)
  set(app_aegis_python_exe ${app_aegis_venv_path}/Scripts/python.exe)
else()
  set(app_aegis_python_exe ${app_aegis_venv_path}/bin/python)
endif()

add_custom_target(
  app_aegis_create_virtualenv
  COMMAND ${CMAKE_COMMAND} -E make_directory ${app_aegis_venv_path}
  COMMAND ${Python3_EXECUTABLE} -m venv ${app_aegis_venv_path}
  COMMENT "Creating Aegis virtual environment"
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
# ----------------------------------------------------------------------- #
# --------------------------- Install Dependencies ---------------------- #
# ----------------------------------------------------------------------- #
set(app_aegis_requirements_file
    ${CMAKE_SOURCE_DIR}/apps/src/aegis/requirements.txt)

add_custom_target(
  app_aegis_install_requirements
  COMMAND ${app_aegis_python_exe} -m pip install --upgrade pip
  COMMAND ${app_aegis_python_exe} -m pip install -r
          ${app_aegis_requirements_file}
  COMMENT "Installing Aegis dependencies from requirements.txt"
  DEPENDS app_aegis_create_virtualenv)
# ----------------------------------------------------------------------- #
# ---------------------------------- Setup ------------------------------ #
# ----------------------------------------------------------------------- #
add_custom_target(
  app_aegis_setup ALL
  COMMENT "Setting up Aegis (creating venv and installing dependencies)"
  DEPENDS app_aegis_install_requirements)
# ----------------------------------------------------------------------- #
