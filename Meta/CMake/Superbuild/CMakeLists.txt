cmake_minimum_required(VERSION 3.16)

project(
    SerenitySuperbuild
    DESCRIPTION "Orchestrate host and target builds in a single build"
    LANGUAGES NONE
)

# NOTE: Before CMake 3.19, if a custom command is attached to multiple step targets for Makefile and Visual Studio generators,
#       it might be run multiple times during the build. Enable new behavior of policy CMP0114 to avoid this, or apply the
#       workaround from https://gitlab.kitware.com/cmake/cmake/-/issues/18663#note_489967
if(NOT CMAKE_VERSION VERSION_LESS "3.19")
  cmake_policy(SET CMP0114 NEW)
  macro(ensure_dependencies)
  endmacro()
else()
  macro(ensure_dependencies proj)
    foreach(step IN ITEMS configure build install)
      if(NOT TARGET "${proj}-${step}")
        ExternalProject_Add_StepTargets("${proj}" "${step}")
      endif()
      if(step STREQUAL "install")
        ExternalProject_Add_StepDependencies("${proj}" install "${proj}-build")
      elseif(step STREQUAL "build")
        ExternalProject_Add_StepDependencies("${proj}" build "${proj}-configure")
      endif()
    endforeach()
  endmacro()
endif()

get_filename_component(
    GELASSENHEIT_SOURCE_DIR "${PROJECT_SOURCE_DIR}/../../.."
    ABSOLUTE CACHE
)
set(GELASSENHEIT_ARCH "i686" CACHE STRING "Target architecture for GelassenheitBS.")
set(GELASSENHEIT_TOOLCHAIN "GNU" CACHE STRING "Compliler toolchain to use for Gelassenheit (GNU or Clang)")

# FIXME: It is preferred to keep all the sub-build artifacts below the binary directory for the superbuild
#        However, this has an impact on developer's IDE settings and more significantly, the Ports tree.
#        See https://github.com/SerenityOS/serenity/pull/9297#discussion_r697877603
set(GELASSENHEIT_BUILD_DIR_SUFFIX "")
if(NOT GELASSENHEIT_TOOLCHAIN STREQUAL "GNU")
  string(TOLOWER "${GELASSENHEIT_TOOLCHAIN}" GELASSENHEIT_BUILD_DIR_SUFFIX)
endif()
set(GELASSENHEIT_BUILD_DIR "${PROJECT_BINARY_DIR}/../${GELASSENHEIT_ARCH}${GELASSENHEIT_BUILD_DIR_SUFFIX}")

if (CMAKE_HOST_SYSTEM_NAME MATCHES "GelassenheitBS")
    message(STATUS "Good job on building cmake!")
else()
    configure_file("${GELASSENHEIT_SOURCE_DIR}/Toolchain/CMake/${GELASSENHEIT_TOOLCHAIN}Toolchain.txt.in" "${GELASSENHEIT_BUILD_DIR}/CMakeToolchain.txt" @ONLY)
    set(GELASSENHEIT_TOOLCHAIN_FILE "${GELASSENHEIT_BUILD_DIR}/CMakeToolchain.txt" CACHE PATH "Toolchain file to use for cross-compilation")
    # Support non-cross builds by stuffing this in a variable
    set(GELASSENHEIT_TOOLCHAIN_FILE_ARG "-DCMAKE_TOOLCHAIN_FILE:STRING=${GELASSENHEIT_TOOLCHAIN_FILE}")
endif()

# Allow the Ninja generators to output messages as they happen by assigning
# these jobs to the 'console' job pool
set(console_access "")
if(CMAKE_GENERATOR MATCHES "^Ninja")
  set(
      console_access
      USES_TERMINAL_CONFIGURE YES
      USES_TERMINAL_BUILD YES
      USES_TERMINAL_INSTALL YES
  )
endif()

include(ExternalProject)

# Collect options for Lagom build
set(lagom_options "")
macro(serenity_option name)
    set(${ARGV})
    list(APPEND lagom_options "-D${name}:STRING=${${name}}")
endmacro()
include("${GELASSENHEIT_SOURCE_DIR}/Meta/CMake/lagom_options.cmake")

# Forward user defined host toolchain to lagom build
if (DEFINED CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER "${CMAKE_C_COMPILER}" CACHE STRING "C Compiler to use for host builds")
    list(APPEND lagom_options "-DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}")
endif()
if (DEFINED CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER "${CMAKE_CXX_COMPILER}" CACHE STRING "C++ Compiler to use for host builds")
    list(APPEND lagom_options "-DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}")
endif()

ExternalProject_Add(
    lagom
    SOURCE_DIR "${GELASSENHEIT_SOURCE_DIR}/Meta/Lagom"
    BINARY_DIR "${PROJECT_BINARY_DIR}/../lagom"
    INSTALL_DIR "${PROJECT_BINARY_DIR}/../lagom-install"
    EXCLUDE_FROM_ALL YES
    CMAKE_CACHE_ARGS
    "-DCMAKE_INSTALL_PREFIX:STRING=<INSTALL_DIR>"
    "-DUCD_PATH:STRING=${GELASSENHEIT_BUILD_DIR}/UCD"
    "-DCLDR_PATH:STRING=${GELASSENHEIT_BUILD_DIR}/CLDR"
    ${lagom_options}
    # Always call the build step of tools, so keeping things up-to-date is easy
    BUILD_ALWAYS YES
    # Expose install step as a target, so it can be depended on
    STEP_TARGETS install
    ${console_access}
)

ensure_dependencies(lagom)

# Collect options for serenity build
set(serenity_options "")
macro(serenity_option name)
    set(${ARGV})
    list(APPEND serenity_options "-D${name}:STRING=${${name}}")
endmacro()
include("${GELASSENHEIT_SOURCE_DIR}/Meta/CMake/serenity_options.cmake")

ExternalProject_Add(
    serenity
    SOURCE_DIR "${GELASSENHEIT_SOURCE_DIR}"
    BINARY_DIR "${GELASSENHEIT_BUILD_DIR}"
    CMAKE_CACHE_ARGS
    # Tell the find_package(Lagom REQUIRED) command call where to find
    # the CMake package
    "-DCMAKE_PREFIX_PATH:STRING=${PROJECT_BINARY_DIR}/../lagom-install"
    "-DUCD_PATH:STRING=${GELASSENHEIT_BUILD_DIR}/UCD"
    "-DCLDR_PATH:STRING=${GELASSENHEIT_BUILD_DIR}/CLDR"
    "-DGELASSENHEIT_ARCH:STRING=${GELASSENHEIT_ARCH}"
    "${GELASSENHEIT_TOOLCHAIN_FILE_ARG}"
    ${serenity_options}
    # Always call the build step
    BUILD_ALWAYS YES
    # Host tools must be built and installed before the OS can be built
    DEPENDS lagom-install
    STEP_TARGETS configure install
    ${console_access}
)

ensure_dependencies(serenity)
