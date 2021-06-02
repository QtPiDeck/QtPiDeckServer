cmake_minimum_required(VERSION 3.15)

project(QtPiDeckServer LANGUAGES CXX)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core)

mark_as_advanced(QT_QMAKE_EXECUTABLE)

include(CTest)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
# qt 6.1.0 has a bug with source_location include with msvc(not sure about other compilers) in c++20 mode
# downgrading to c++17 leads to a few problem resolved in code by checking qt version
if (MSVC AND ${QT_VERSION_MAJOR} EQUAL 6 AND ${QT_VERSION_MINOR} EQUAL 1 AND ${QT_VERSION_PATCH} EQUAL 0)
set(CMAKE_CXX_STANDARD 17)
else()
set(CMAKE_CXX_STANDARD 20)
endif()
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

# qt6 bindings
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" AND ${QT_VERSION_MAJOR} EQUAL 6)
    add_compile_options(-Wno-invalid-offsetof)
endif()

# concepts
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" AND ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 10.0)
    add_compile_options(-fconcepts)
endif()

add_subdirectory(QtPiDeckCommon)
add_subdirectory(QtPiDeckServer)
add_subdirectory(Tests)
