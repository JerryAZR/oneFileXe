# Generated by CMake

if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 2.6)
   message(FATAL_ERROR "CMake >= 2.6.0 required")
endif()
cmake_policy(PUSH)
cmake_policy(VERSION 2.6...3.21)
#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Protect against multiple inclusion, which would fail when already imported targets are added once more.
set(_targetsDefined)
set(_targetsNotDefined)
set(_expectedTargets)
foreach(_expectedTarget Qt6::Platform Qt6::GlobalConfig Qt6::GlobalConfigPrivate Qt6::PlatformCommonInternal Qt6::PlatformModuleInternal Qt6::PlatformPluginInternal Qt6::PlatformAppInternal Qt6::PlatformToolInternal)
  list(APPEND _expectedTargets ${_expectedTarget})
  if(NOT TARGET ${_expectedTarget})
    list(APPEND _targetsNotDefined ${_expectedTarget})
  endif()
  if(TARGET ${_expectedTarget})
    list(APPEND _targetsDefined ${_expectedTarget})
  endif()
endforeach()
if("${_targetsDefined}" STREQUAL "${_expectedTargets}")
  unset(_targetsDefined)
  unset(_targetsNotDefined)
  unset(_expectedTargets)
  set(CMAKE_IMPORT_FILE_VERSION)
  cmake_policy(POP)
  return()
endif()
if(NOT "${_targetsDefined}" STREQUAL "")
  message(FATAL_ERROR "Some (but not all) targets in this export set were already defined.\nTargets Defined: ${_targetsDefined}\nTargets not yet defined: ${_targetsNotDefined}\n")
endif()
unset(_targetsDefined)
unset(_targetsNotDefined)
unset(_expectedTargets)


# Compute the installation prefix relative to this file.
get_filename_component(_IMPORT_PREFIX "${CMAKE_CURRENT_LIST_FILE}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)
get_filename_component(_IMPORT_PREFIX "${_IMPORT_PREFIX}" PATH)
if(_IMPORT_PREFIX STREQUAL "/")
  set(_IMPORT_PREFIX "")
endif()

# Create imported target Qt6::Platform
add_library(Qt6::Platform INTERFACE IMPORTED)

set_target_properties(Qt6::Platform PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "WIN32;_ENABLE_EXTENDED_ALIGNED_STORAGE;WIN64;_WIN64;MINGW_HAS_SECURE_API=1;\$<\$<NOT:\$<BOOL:\$<TARGET_PROPERTY:QT_NO_UNICODE_DEFINES>>>:UNICODE;_UNICODE>"
  INTERFACE_COMPILE_FEATURES "cxx_std_17"
  INTERFACE_INCLUDE_DIRECTORIES "${_IMPORT_PREFIX}/mkspecs/win32-g++;${_IMPORT_PREFIX}/include"
  INTERFACE_LINK_LIBRARIES "Threads::Threads"
)

# Create imported target Qt6::GlobalConfig
add_library(Qt6::GlobalConfig INTERFACE IMPORTED)

set_target_properties(Qt6::GlobalConfig PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${_IMPORT_PREFIX}/include;${_IMPORT_PREFIX}/include/QtCore"
)

# Create imported target Qt6::GlobalConfigPrivate
add_library(Qt6::GlobalConfigPrivate INTERFACE IMPORTED)

set_target_properties(Qt6::GlobalConfigPrivate PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES "${_IMPORT_PREFIX}/include/QtCore/6.3.0;${_IMPORT_PREFIX}/include/QtCore/6.3.0/QtCore"
  INTERFACE_LINK_LIBRARIES "Qt6::GlobalConfig"
)

# Create imported target Qt6::PlatformCommonInternal
add_library(Qt6::PlatformCommonInternal INTERFACE IMPORTED)

set_target_properties(Qt6::PlatformCommonInternal PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "\$<\$<NOT:\$<BOOL:\$<TARGET_PROPERTY:QT_INTERNAL_UNDEF_QT_NO_JAVA_STYLE_ITERATORS>>>:QT_NO_JAVA_STYLE_ITERATORS>;\$<\$<NOT:\$<BOOL:\$<TARGET_PROPERTY:QT_INTERNAL_UNDEF_QT_NO_NARROWING_CONVERSIONS_IN_CONNECT>>>:QT_NO_NARROWING_CONVERSIONS_IN_CONNECT>;QT_NO_NARROWING_CONVERSIONS_IN_CONNECT;\$<\$<NOT:\$<CONFIG:Debug>>:QT_NO_DEBUG>"
  INTERFACE_COMPILE_OPTIONS "-Wa,-mbig-obj;\$<\$<COMPILE_LANGUAGE:CXX>:-Wsuggest-override>"
  INTERFACE_LINK_LIBRARIES "Qt6::Platform"
)

# Create imported target Qt6::PlatformModuleInternal
add_library(Qt6::PlatformModuleInternal INTERFACE IMPORTED)

set_target_properties(Qt6::PlatformModuleInternal PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS "_USE_MATH_DEFINES"
  INTERFACE_LINK_LIBRARIES "Qt6::PlatformCommonInternal"
)

# Create imported target Qt6::PlatformPluginInternal
add_library(Qt6::PlatformPluginInternal INTERFACE IMPORTED)

set_target_properties(Qt6::PlatformPluginInternal PROPERTIES
  INTERFACE_LINK_LIBRARIES "Qt6::PlatformCommonInternal"
)

# Create imported target Qt6::PlatformAppInternal
add_library(Qt6::PlatformAppInternal INTERFACE IMPORTED)

set_target_properties(Qt6::PlatformAppInternal PROPERTIES
  INTERFACE_LINK_LIBRARIES "Qt6::PlatformCommonInternal"
)

# Create imported target Qt6::PlatformToolInternal
add_library(Qt6::PlatformToolInternal INTERFACE IMPORTED)

set_target_properties(Qt6::PlatformToolInternal PROPERTIES
  INTERFACE_LINK_LIBRARIES "Qt6::PlatformAppInternal"
)

if(CMAKE_VERSION VERSION_LESS 3.0.0)
  message(FATAL_ERROR "This file relies on consumers using CMake 3.0.0 or greater.")
endif()

# Load information for each installed configuration.
get_filename_component(_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
file(GLOB CONFIG_FILES "${_DIR}/Qt6Targets-*.cmake")
foreach(f ${CONFIG_FILES})
  include(${f})
endforeach()

# Cleanup temporary variables.
set(_IMPORT_PREFIX)

# Loop over all imported files and verify that they actually exist
foreach(target ${_IMPORT_CHECK_TARGETS} )
  foreach(file ${_IMPORT_CHECK_FILES_FOR_${target}} )
    if(NOT EXISTS "${file}" )
      message(FATAL_ERROR "The imported target \"${target}\" references the file
   \"${file}\"
but this file does not exist.  Possible reasons include:
* The file was deleted, renamed, or moved to another location.
* An install or uninstall procedure did not complete successfully.
* The installation package was faulty and contained
   \"${CMAKE_CURRENT_LIST_FILE}\"
but not all the files it references.
")
    endif()
  endforeach()
  unset(_IMPORT_CHECK_FILES_FOR_${target})
endforeach()
unset(_IMPORT_CHECK_TARGETS)

# This file does not depend on other imported targets which have
# been exported from the same project but in a separate export set.

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
cmake_policy(POP)
