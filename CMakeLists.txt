cmake_minimum_required(VERSION 3.15)

project(QtPiDeckServer LANGUAGES CXX)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core)

mark_as_advanced(QT_QMAKE_EXECUTABLE)

include(CTest)

if(MSVC AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.14)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /experimental:external /external:W0 ")
    set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "/external:I ")
endif()

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# qt6 bindings
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" AND ${QT_VERSION_MAJOR} EQUAL 6)
    add_compile_options(-Wno-invalid-offsetof)
endif()

message(STATUS "Performing Test HAVE_CXX20RANGES")
try_compile(CXX20RANGES ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}/try_compile/ranges.cpp)
if (CXX20RANGES)
    message(STATUS "Performing Test HAVE_CXX20RANGES - Success")
else()
    message(STATUS "Performing Test HAVE_CXX20RANGES - Fail")
    message(STATUS "range-v3 will be used")
endif()

# concepts
if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" AND ${CMAKE_CXX_COMPILER_VERSION} VERSION_LESS 10.0)
    add_compile_options(-fconcepts)
endif()

add_subdirectory(QtPiDeckCommon)
add_subdirectory(QtPiDeckServer)
add_subdirectory(Tests)
