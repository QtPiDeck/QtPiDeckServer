cmake_minimum_required(VERSION 3.14)

project(QtPiDeckServer LANGUAGES CXX)

include(CTest)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(QtPiDeckCommon)
add_subdirectory(QtPiDeckServer)
