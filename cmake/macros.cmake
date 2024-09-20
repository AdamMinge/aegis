# ----------------------------------------------------------------------- #
# -------------- Define a macro that prepare target module -------------- #
# ----------------------------------------------------------------------- #
macro(__aegis_prepare_module_target target)
  string(REPLACE "-" "_" NAME_UPPER "${target}")
  string(TOUPPER "${NAME_UPPER}" NAME_UPPER)
  set_target_properties(${target} PROPERTIES DEFINE_SYMBOL
                                             ${NAME_UPPER}_EXPORTS)

  if(BUILD_SHARED_LIBS)
    set_target_properties(${target} PROPERTIES DEBUG_POSTFIX -d)
  else()
    set_target_properties(${target} PROPERTIES DEBUG_POSTFIX -s-d)
    set_target_properties(${target} PROPERTIES RELEASE_POSTFIX -s)
  endif()

  set_target_properties(${target} PROPERTIES COMPILE_FEATURES cxx_std_20)
  set_target_properties(${target} PROPERTIES LINKER_LANGUAGE CXX)

  install(
    TARGETS ${target}
    EXPORT aegisConfigExport
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT bin
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT lib
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT lib)

endmacro()
# ----------------------------------------------------------------------- # ----
# Define a macro that create proto libs and link it to target ------ #
# ----------------------------------------------------------------------- #
macro(aegis_add_proto_modules target)
  cmake_parse_arguments(THIS "" "" "PROTOS" ${ARGN})
  if(NOT "${THIS_UNPARSED_ARGUMENTS}" STREQUAL "")
    message(
      FATAL_ERROR
        "Extra unparsed arguments when calling aegis_add_proto_modules: ${THIS_UNPARSED_ARGUMENTS}"
    )
  endif()

  find_package(Protobuf REQUIRED)
  find_package(gRPC REQUIRED)

  if(NOT gRPC_CPP_PLUGIN)
    find_program(
      gRPC_CPP_PLUGIN
      NAMES grpc_cpp_plugin
      HINTS ${gRPC_ROOT} ENV PATH
      DOC "Path to grpc_cpp_plugin")

    if(NOT gRPC_CPP_PLUGIN)
      message(FATAL_ERROR "grpc_cpp_plugin executable not found.")
    else()
      message(STATUS "Using manually found gRPC C++ plugin: ${gRPC_CPP_PLUGIN}")
    endif()
  endif()

  foreach(PROTO_FILE ${THIS_PROTOS})
    get_filename_component(PROTO_FILENAME_WE ${PROTO_FILE} NAME_WE)
    get_filename_component(PROTO_DIRECTORY ${PROTO_FILE} DIRECTORY)

    set(PROTO_TARGET ${PROTO_FILENAME_WE}_proto)
    set(GENERATED_PROTO_DIR
        ${CMAKE_CURRENT_BINARY_DIR}/generated/${PROTO_TARGET})

    set(PROTO_SRC ${GENERATED_PROTO_DIR}/${PROTO_FILENAME_WE}.pb.cc)
    set(PROTO_HDR ${GENERATED_PROTO_DIR}/${PROTO_FILENAME_WE}.pb.h)
    set(GRPC_SRC ${GENERATED_PROTO_DIR}/${PROTO_FILENAME_WE}.grpc.pb.cc)
    set(GRPC_HDR ${GENERATED_PROTO_DIR}/${PROTO_FILENAME_WE}.grpc.pb.h)

    add_custom_target(
      create_generated_dir ALL
      COMMAND ${CMAKE_COMMAND} -E make_directory ${GENERATED_PROTO_DIR}
      COMMENT "Creating directory ${GENERATED_PROTO_DIR}")

    add_custom_command(
      OUTPUT ${PROTO_SRC} ${PROTO_HDR} ${GRPC_SRC} ${GRPC_HDR}
      COMMAND
        ${Protobuf_PROTOC_EXECUTABLE} ARGS --proto_path=${PROTO_DIRECTORY}
        --cpp_out=${GENERATED_PROTO_DIR} --grpc_out=${GENERATED_PROTO_DIR}
        --plugin=protoc-gen-grpc=${gRPC_CPP_PLUGIN} ${PROTO_FILE}
      DEPENDS ${PROTO_FILE}
      COMMENT "Generating C++ code from ${PROTO_FILE}")

    add_library(${PROTO_TARGET} ${PROTO_SRC} ${PROTO_HDR} ${GRPC_SRC}
                                ${GRPC_HDR})

    target_link_libraries(${PROTO_TARGET} PRIVATE protobuf::libprotobuf
                                                  gRPC::grpc++)

    __aegis_prepare_module_target(${PROTO_TARGET})

    target_include_directories(
      ${PROTO_TARGET}
      PUBLIC $<BUILD_INTERFACE:${GENERATED_PROTO_DIR}/..>
      INTERFACE $<INSTALL_INTERFACE:include>)

    target_link_libraries(${target} PRIVATE ${PROTO_TARGET})

    add_dependencies(${PROTO_TARGET} create_generated_dir)

  endforeach()

  target_link_libraries(${target} PRIVATE protobuf::libprotobuf gRPC::grpc++)

endmacro()
# ----------------------------------------------------------------------- #
# ---------- Define a macro that helps add headers only module ---------- #
# ----------------------------------------------------------------------- #
macro(aegis_add_headers_only_module target)

  cmake_parse_arguments(THIS "" "" "SOURCES;DEPENDS" ${ARGN})
  if(NOT "${THIS_UNPARSED_ARGUMENTS}" STREQUAL "")
    message(
      FATAL_ERROR
        "Extra unparsed arguments when calling aegis_add_headers_only_module: ${THIS_UNPARSED_ARGUMENTS}"
    )
  endif()

  add_library(${target} INTERFACE ${THIS_SOURCES})
  add_library(aegis::${target} ALIAS ${target})

  __aegis_prepare_module_target(${target})

  if(THIS_DEPENDS)
    target_link_libraries(${target} INTERFACE ${THIS_DEPENDS})
  endif()

  if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(${target} INTERFACE "aegis_STATIC")
  endif()

endmacro()
# ----------------------------------------------------------------------- #
# ----------------- Define a macro that helps add module ---------------- #
# ----------------------------------------------------------------------- #
macro(aegis_add_module target)
  cmake_parse_arguments(
    THIS
    ""
    ""
    "SOURCES;PROTOS;DEPENDS;DEPENDS_PRIVATE;PRECOMPILE_HEADERS;PRECOMPILE_PRIVATE_HEADERS"
    ${ARGN})
  if(NOT "${THIS_UNPARSED_ARGUMENTS}" STREQUAL "")
    message(
      FATAL_ERROR
        "Extra unparsed arguments when calling aegis_add_module: ${THIS_UNPARSED_ARGUMENTS}"
    )
  endif()

  add_library(${target} ${THIS_SOURCES})
  add_library(aegis::${target} ALIAS ${target})

  __aegis_prepare_module_target(${target})

  if(THIS_PROTOS)
    aegis_add_proto_modules(${target} PROTOS ${THIS_PROTOS})
  endif()

  if(THIS_DEPENDS)
    target_link_libraries(${target} PUBLIC ${THIS_DEPENDS})
  endif()

  if(THIS_DEPENDS_PRIVATE)
    target_link_libraries(${target} PRIVATE ${THIS_DEPENDS_PRIVATE})
  endif()

  if(THIS_PRECOMPILE_HEADERS)
    target_precompile_headers(${target} PUBLIC ${THIS_PRECOMPILE_HEADERS})
  endif()

  if(THIS_PRECOMPILE_PRIVATE_HEADERS)
    target_precompile_headers(${target} PRIVATE
                              ${THIS_PRECOMPILE_PRIVATE_HEADERS})
  endif()

  if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(${target} PUBLIC "aegis_STATIC")
  endif()

endmacro()
# ----------------------------------------------------------------------- #
# --------------- Define a macro that helps export engine --------------- #
# ----------------------------------------------------------------------- #
function(aegis_export_modules)

  if(BUILD_SHARED_LIBS)
    set(config_name "shared")
  else()
    set(config_name "static")
  endif()

  set(current_dir "${AEGIS_SOURCE_DIR}/cmake")
  set(targets_config_filename "aegis-${config_name}-targets.cmake")
  set(config_package_location ${CMAKE_INSTALL_LIBDIR}/cmake/aegis)

  include(CMakePackageConfigHelpers)
  write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/aegis-config-version.cmake"
    VERSION ${AEGIS_VERSION_MAJOR}.${AEGIS_VERSION_MINOR}
    COMPATIBILITY SameMajorVersion)

  export(EXPORT aegisConfigExport
         FILE "${CMAKE_CURRENT_BINARY_DIR}/${targets_config_filename}")

  configure_package_config_file(
    "${current_dir}/aegis-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/aegis-config.cmake"
    INSTALL_DESTINATION "${config_package_location}")
  configure_package_config_file(
    "${current_dir}/aegis-config-dependencies.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/aegis-config-dependencies.cmake"
    INSTALL_DESTINATION "${config_package_location}")

  install(
    EXPORT aegisConfigExport
    FILE ${targets_config_filename}
    NAMESPACE aegis::
    DESTINATION ${config_package_location})

  install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/aegis-config.cmake
          ${CMAKE_CURRENT_BINARY_DIR}/aegis-config-dependencies.cmake
          ${CMAKE_CURRENT_BINARY_DIR}/aegis-config-version.cmake
    DESTINATION ${config_package_location})

  install(
    DIRECTORY ${AEGIS_SOURCE_DIR}/modules/include
    DESTINATION .
    COMPONENT devel
    FILES_MATCHING
    PATTERN "*.hpp"
    PATTERN "*.inl"
    PATTERN "*.h")

endfunction()
# ----------------------------------------------------------------------- #
# -------------- Define a macro that helps add application -------------- #
# ----------------------------------------------------------------------- #
macro(__aegis_add_executable target)

  cmake_parse_arguments(THIS "" "RESOURCES_DIR"
                        "SOURCES;DEPENDS;DEPENDS_PRIVATE" ${ARGN})
  if(NOT "${THIS_UNPARSED_ARGUMENTS}" STREQUAL "")
    message(
      FATAL_ERROR
        "Extra unparsed arguments when calling __aegis_add_executable: ${THIS_UNPARSED_ARGUMENTS}"
    )
  endif()

  add_executable(${target} ${THIS_SOURCES})
  set_target_properties(${target} PROPERTIES DEBUG_POSTFIX -d)

  if(THIS_DEPENDS)
    target_link_libraries(${target} PUBLIC ${THIS_DEPENDS})
  endif()

  if(THIS_DEPENDS_PRIVATE)
    target_link_libraries(${target} PRIVATE ${THIS_DEPENDS_PRIVATE})
  endif()

  install(TARGETS ${target} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
                                    COMPONENT bin)

  if(THIS_RESOURCES_DIR)
    get_filename_component(THIS_RESOURCES_DIR "${THIS_RESOURCES_DIR}" ABSOLUTE)

    if(NOT EXISTS "${THIS_RESOURCES_DIR}")
      message(
        FATAL_ERROR
          "Given resources directory to install does not exist: ${THIS_RESOURCES_DIR}"
      )
    endif()
    install(DIRECTORY ${THIS_RESOURCES_DIR}
            DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/${target})
  endif()

  set_target_properties(${target} PROPERTIES CXX_STANDARD 20)

endmacro()
# ----------------------------------------------------------------------- #
# ----------------- Define a macro that helps add tool ------------------ #
# ----------------------------------------------------------------------- #
macro(aegis_add_application target)

  cmake_parse_arguments(THIS "" "RESOURCES_DIR"
                        "SOURCES;DEPENDS;DEPENDS_PRIVATE" ${ARGN})
  if(NOT "${THIS_UNPARSED_ARGUMENTS}" STREQUAL "")
    message(
      FATAL_ERROR
        "Extra unparsed arguments when calling aegis_add_application: ${THIS_UNPARSED_ARGUMENTS}"
    )
  endif()

  __aegis_add_executable(
    ${target}
    SOURCES
    ${THIS_SOURCES}
    DEPENDS
    ${THIS_DEPENDS}
    DEPENDS_PRIVATE
    ${THIS_DEPENDS_PRIVATE}
    RESOURCES_DIR
    ${THIS_RESOURCES_DIR})

  target_compile_definitions(${target} PUBLIC QT_NO_KEYWORDS)

endmacro()
# ----------------------------------------------------------------------- #
# ----------- Define a function that helps add static analysis ---------- #
# ----------------------------------------------------------------------- #
function(aegis_static_analyzers)
  if(AEGIS_ENABLE_CLANG_TIDY)
    find_program(CLANGTIDY clang-tidy)
    if(CLANGTIDY)
      set(CMAKE_CXX_CLANG_TIDY ${CLANGTIDY}
                               -extra-arg=-Wno-unknown-warning-option)
    else()
      message(FATAL_ERROR "Clang-Tidy requested but executable not found")
    endif()
  endif()

  if(AEGIS_ENABLE_CPPCHECK)
    find_program(CPPCHECK cppcheck)
    if(CPPCHECK)
      set(CMAKE_CXX_CPPCHECK ${CPPCHECK} --suppress=missingInclude --enable=all
                             --inline-suppr --inconclusive --force)
    else()
      message(FATAL_ERROR "Cppcheck requested but executable not found")
    endif()
  endif()
endfunction()
# ----------------------------------------------------------------------- #
# -------------- Define a macro that helps add python venv -------------- #
# ----------------------------------------------------------------------- #
macro(__aegis_create_python_venv)
  cmake_parse_arguments(THIS "" "VENV;WORKING_DIRECTORY" "" ${ARGN})
  if(NOT "${THIS_UNPARSED_ARGUMENTS}" STREQUAL "")
    message(
      FATAL_ERROR
        "Extra unparsed arguments when calling aegis_python_venv: ${THIS_UNPARSED_ARGUMENTS}"
    )
  endif()

  if(NOT EXISTS ${THIS_VENV})
    find_package(Python 3.10 REQUIRED)
    execute_process(COMMAND ${Python_EXECUTABLE} "-m" "venv" ${THIS_VENV}
                    WORKING_DIRECTORY ${THIS_WORKING_DIRECTORY})
    unset(${Python_EXECUTABLE})
  endif()

  if(AEGIS_OS_WINDOWS)
    set(Python_EXECUTABLE ${THIS_VENV}/Scripts/python)
  else()
    set(Python_EXECUTABLE ${THIS_VENV}/bin/python)
  endif()

  find_package(Python 3.10 REQUIRED)

  execute_process(COMMAND ${Python_EXECUTABLE} "-m" "pip" "install" "--upgrade"
                          "pip" WORKING_DIRECTORY ${THIS_WORKING_DIRECTORY})

  execute_process(COMMAND ${Python_EXECUTABLE} "-m" "pip" "install" "poetry"
                  WORKING_DIRECTORY ${THIS_WORKING_DIRECTORY})

  if(AEGIS_OS_WINDOWS)
    set(poetry ${venv_path}/Scripts/poetry.exe)
  else()
    set(poetry ${venv_path}/bin/poetry)
  endif()

  execute_process(
    COMMAND ${CMAKE_COMMAND} -E env VIRTUAL_ENV=${venv_path} ${poetry} "install"
    WORKING_DIRECTORY ${THIS_WORKING_DIRECTORY})
endmacro()
# ----------------------------------------------------------------------- #
# -------------- Define a macro that helps add python module ------------ #
# ----------------------------------------------------------------------- #
macro(aegis_add_python_module target)
  cmake_parse_arguments(THIS "" "" "" ${ARGN})
  if(NOT "${THIS_UNPARSED_ARGUMENTS}" STREQUAL "")
    message(
      FATAL_ERROR
        "Extra unparsed arguments when calling aegis_python_venv: ${THIS_UNPARSED_ARGUMENTS}"
    )
  endif()

  set(venv_path ${CMAKE_CURRENT_BINARY_DIR}/venv)

  __aegis_create_python_venv(VENV ${venv_path} WORKING_DIRECTORY
                             ${CMAKE_CURRENT_SOURCE_DIR})

endmacro()
# ----------------------------------------------------------------------- #
# --------------- Define a macro that helps add python app -------------- #
# ----------------------------------------------------------------------- #
macro(aegis_add_python_app target)
  cmake_parse_arguments(THIS "" "SOURCE;QRC" "" ${ARGN})
  if(NOT "${THIS_UNPARSED_ARGUMENTS}" STREQUAL "")
    message(
      FATAL_ERROR
        "Extra unparsed arguments when calling aegis_python_venv: ${THIS_UNPARSED_ARGUMENTS}"
    )
  endif()

  set(venv_path ${CMAKE_CURRENT_BINARY_DIR}/venv)

  __aegis_create_python_venv(VENV ${venv_path} WORKING_DIRECTORY
                             ${CMAKE_CURRENT_SOURCE_DIR})

  if(AEGIS_OS_WINDOWS)
    set(pyside6_rcc ${venv_path}/Scripts/pyside6-rcc.exe)
    set(pyside6_deploy ${venv_path}/Scripts/pyside6-deploy.exe)
  else()
    set(pyside6_rcc ${venv_path}/bin/pyside6-rcc)
    set(pyside6_deploy ${venv_path}/bin/pyside6-deploy)
  endif()

  add_custom_target(
    ${target}_build_rcc
    COMMAND ${CMAKE_COMMAND} -E env VIRTUAL_ENV=${venv_path} ${pyside6_rcc}
            ${THIS_QRC} -o ${THIS_SOURCE}/${target}/__generated__/rcc.py
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Build ${target} resources")

  configure_file(${THIS_SOURCE}/pysidedeploy.spec.in
                 ${THIS_SOURCE}/${target}/__generated__/pysidedeploy.spec)

  add_custom_target(
    ${target}_setup ALL
    COMMENT
      "Setting up ${target} (creating venv, installing dependencies and build rcc)"
    DEPENDS ${target}_build_rcc)

  add_custom_target(
    ${target}_deploy
    COMMAND
      ${CMAKE_COMMAND} -E env VIRTUAL_ENV=${venv_path} ${pyside6_deploy}
      ${THIS_SOURCE}/${target}/main.py -c
      ${THIS_SOURCE}/${target}/__generated__/pysidedeploy.spec
    WORKING_DIRECTORY ${THIS_SOURCE}/${target}
    COMMENT "Deploing ${target}")

  install(
    CODE "execute_process(COMMAND ${CMAKE_COMMAND} --build . --target ${target}_deploy)"
  )

endmacro()
# ----------------------------------------------------------------------- #
