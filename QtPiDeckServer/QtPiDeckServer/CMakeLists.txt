find_package(QT NAMES Qt6 Qt5 COMPONENTS Quick REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Quick REQUIRED)

add_executable(QtPiDeckServer
    src/main.cpp)

target_link_libraries(QtPiDeckServer
    PRIVATE
    Qt${QT_VERSION_MAJOR}::Quick
    QtPiDeckServerLib)

target_include_directories(QtPiDeckServer
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_warnings_flags(QtPiDeckServer)