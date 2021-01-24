cmake_minimum_required(VERSION 3.15)

project(QtPiDeckServer LANGUAGES CXX)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core)

include(CTest)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FLAGS_SET_BY_PARENT 1)
if (MSVC)
    # warning level 4 and all warnings as errors
    # waiting for /external to be fixed
    #add_compile_options(/W4 /WX)
    add_compile_options(/WX)
else()
    # lots of warnings and all warnings as errors
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()

#qt6 bindings
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND ${QT_VERSION_MAJOR} EQUAL 6)
    add_compile_options(-Wno-invalid-offsetof)
endif()

add_subdirectory(QtPiDeckCommon)
add_subdirectory(QtPiDeckServer)
add_subdirectory(Tests)
