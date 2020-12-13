find_package(QT NAMES Qt6 Qt5 COMPONENTS Quick Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Quick Network REQUIRED)

add_library(QtPiDeckServerLib STATIC
  src/ServerApplication.cpp
  include/ServerApplication.hpp
  src/Network/DeckServer.cpp
  include/Network/DeckServer.hpp
)

target_include_directories(QtPiDeckServerLib
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(QtPiDeckServerLib
    PUBLIC
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Network
    QtPiDeckCommon
    PRIVATE)
