# ----------------------------------------------------------------------- #
# ---------------------------- Set CMake Version ------------------------ #
# ----------------------------------------------------------------------- #
cmake_minimum_required(VERSION 3.11)
# ----------------------------------------------------------------------- #
# ---------------------------- Set files variable ----------------------- #
# ----------------------------------------------------------------------- #
set(module_root ${AEGIS_SOURCE_DIR}/apps/src/aegis_ide/aegis_ide)
set(resource_root ${AEGIS_SOURCE_DIR}/apps/resource/aegis_ide)
# ----------------------------------------------------------------------- #
# -------------------------------- Setup VENV --------------------------- #
# ----------------------------------------------------------------------- #
aegis_add_python_app(pyaegis_ide MODULE ${module_root} QRC
                     ${resource_root}/aegis_ide.qrc)
# ----------------------------------------------------------------------- #
