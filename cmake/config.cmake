# ----------------------------------------------------------------------- #
# ----------- Define a macro that helps defining an option -------------- #
# ----------------------------------------------------------------------- #
macro(aegis_set_option var default type docstring)
  if(NOT DEFINED ${var})
    set(${var} ${default})
  endif()
  set(${var}
      ${${var}}
      CACHE ${type} ${docstring} FORCE)
endmacro()
# ----------------------------------------------------------------------- #
# ---------------------------- Detect the OS ---------------------------- #
# ----------------------------------------------------------------------- #
if(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
  set(AEGIS_OS_WINDOWS TRUE)
elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  set(AEGIS_OS_LINUX TRUE)
else()
  message(FATAL_ERROR "Unsupported operating system or environment")
endif()
# ----------------------------------------------------------------------- #
# ---------------------------- Define options --------------------------- #
# ----------------------------------------------------------------------- #
aegis_set_option(
  BUILD_SHARED_LIBS
  TRUE
  BOOL
  "TRUE to build AEGIS dependencies as shared libraries, FALSE to build it as static libraries"
)
aegis_set_option(
  AEGIS_DEPRECATED_WARNINGS TRUE BOOL
  "FALSE to disable deprecated warning, TRUE to enable depracated warning")
aegis_set_option(AEGIS_BUILD_DOCUMENTATION FALSE BOOL
                 "TRUE to build the documentation, FALSE to ignore them")
aegis_set_option(AEGIS_ENABLE_CLANG_TIDY FALSE BOOL
                 "TRUE to enable clang tidy, FALSE to ignore them")
aegis_set_option(AEGIS_ENABLE_CPPCHECK FALSE BOOL
                 "TRUE to enable cppcheck, FALSE to ignore them")
# ----------------------------------------------------------------------- #
# -------------------------- Set other options -------------------------- #
# ----------------------------------------------------------------------- #
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${AEGIS_SOURCE_DIR}/cmake/modules)
# ----------------------------------------------------------------------- #
