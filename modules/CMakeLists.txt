# ----------------------------------------------------------------------- #
# ---------------------------- Set CMake Version ------------------------ #
# ----------------------------------------------------------------------- #
cmake_minimum_required(VERSION 3.11)
# ----------------------------------------------------------------------- #
# ---------------------------- Generate headers ------------------------- #
# ----------------------------------------------------------------------- #
set(include_root ${AEGIS_SOURCE_DIR}/modules/include/aegis)
configure_file(${include_root}/config.h.in ${include_root}/config.h)
# ----------------------------------------------------------------------- #
# ---------------------------- Set files variable ----------------------- #
# ----------------------------------------------------------------------- #
add_subdirectory(src/aegis)
# ----------------------------------------------------------------------- #
