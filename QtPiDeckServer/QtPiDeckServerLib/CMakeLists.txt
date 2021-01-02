find_package(QT NAMES Qt6 Qt5 COMPONENTS Quick Network REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Quick Network REQUIRED)

add_library(QtPiDeckServerLib STATIC
  src/ServerApplication.cpp
  src/Network/DeckServer.cpp
  src/Services/SettingsStorage.cpp
  include/ServerApplication.hpp
  include/Network/DeckServer.hpp
  include/Services/IServerSettingsStorage.hpp
  include/Services/SettingsStorage.hpp
  qml.qrc
  )

target_include_directories(QtPiDeckServerLib
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(QtPiDeckServerLib
    PUBLIC
    Qt${QT_VERSION_MAJOR}::Quick
    Qt${QT_VERSION_MAJOR}::Network
    QtPiDeckCommon
    PRIVATE)
