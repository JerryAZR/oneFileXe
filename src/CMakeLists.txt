cmake_minimum_required(VERSION 3.14)

project(onefilexe LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

get_target_property(target_type Qt6::Core TYPE)
if (target_type STREQUAL STATIC_LIBRARY)
    if (MSVC)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
        set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MT")
        set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} /MT")
    else()
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
    endif()
endif()

add_executable(base
    loader/main.cpp
)

add_executable(onefilexe
  main.cpp
  packer.h
  packer.cpp

  data.qrc
)
target_link_libraries(base Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(onefilexe Qt${QT_VERSION_MAJOR}::Core)

add_custom_command(
    OUTPUT ${CMAKE_SOURCE_DIR}/loader/bin/base
    DEPENDS base
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy $<TARGET_FILE:base> ${CMAKE_SOURCE_DIR}/loader/bin/base
)
